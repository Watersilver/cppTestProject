#ifndef ECS_H
#define ECS_H

#include <vector>
#include <array>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <bitset>
#include <tuple>
#include <type_traits>
#include <stdexcept>

#include <iostream>

struct Component {};

struct UniqueComponent : Component {};

using EntityIdType = unsigned long long int;
class EntityConnection
{
  EntityIdType _id{ 0 };
  std::unordered_multimap<EntityIdType, EntityConnection*>* cons{ nullptr };
  bool connected{ false };

  void disconnect()
  {
    if (_id == 0 || !cons) return;

    auto range = cons->equal_range(_id);
    for (auto it = range.first; it != range.second; ++it) {
      if (it->second != this) continue;
      cons->erase(it);
      break;
    }

    std::cout << "disconnected: " << cons->size() << '\n';
  }
public:
  ~EntityConnection()
  {
    disconnect();
  }

  bool connect(std::unordered_multimap<EntityIdType, EntityConnection*>* consMultimap)
  {
    if (connected) return false;
    cons = consMultimap;

    connected = true;
    return true;
  }

  void setId(EntityIdType newId)
  {
    if (!cons) return;
    disconnect();

    _id = newId;

    // Connect new id
    if (_id == 0) return;
    //cons.insert(_id, this);
    cons->insert(std::make_pair(_id, this));

    std::cout << "connected: " << cons->size() << '\n';
  }

  EntityIdType id() { return _id; }
};

// Very simple naive implementation of ecs to
// check out how its interface feels for game making
template <typename ...ComponentTypes>
class Ecs
{
	using ComponentId = unsigned int;
public:
	using EntityId = EntityIdType;
  // This signature is the only reason I made this a variadic template
  // Terrible idea in hindsight.
	using Signature = std::bitset<sizeof...(ComponentTypes)>;
  using ComponentPool = std::unordered_map<EntityId, std::shared_ptr<Component>>;

  std::unordered_multimap<EntityId, EntityConnection*> entityConnections{};

	Ecs()
	{
		((getSignature<ComponentTypes>()), ...);
	}

	template <typename ComponentType>
	Signature getSignature()
	{
		static Signature mySignature{ ((Signature)1) << getCompID<ComponentType>() };
		return mySignature;
	}

	class System
	{
	private:
		virtual void update(EntityId entityID, float deltaT) = 0;
	protected:
		bool firstTime{ true };
		bool loopStart{ true };
	public:
		void updateAll(const std::unordered_set<EntityId>& sysEnts, const float deltaT)
		{
			//std::cout << "Running system. Entities: ";
			for (auto& ent : sysEnts)
			{
				//std::cout << '|';
				// If signature is subset of entity signature, run system
				// entity will only get added to sysEnts if that is so
				//std::cout << "ent is " << ent << '\n';
				update(ent, deltaT);
				firstTime = false;
				loopStart = false;
			}
			//std::cout << '\n';
			loopStart = true;
		}

		// Not really meant to ever be destroyed
		virtual ~System() {};
	};

	void runSystems(float deltaT)
	{
		//std::cout << "Running systems" << '\n';
		for (auto& sysTuple : registeredSystems)
		{
			// determine dt here if I need it.
			// Update all entities that belong to a system.
			// the third entry of the registered systems tuple
			// holds them. Pass them to updateAll.
			// Entities might or might not have been better as a member of system but whatever.
			std::get<0>(sysTuple)->updateAll(std::get<2>(sysTuple), deltaT);
		}

		// Add remove components and entities here,
		// to avoid weirdness if I do that mid iteration.
		refreshSysEnts();
	}

	template <typename SystemSubclass>
	bool registerSystem()
	{
		// Make sure this only runs once per system
		static unsigned int sizeAfterReg = regSysNum++;
		if (sizeAfterReg < registeredSystems.size()) return false;
		registeredSystems.push_back(
			std::make_tuple(
				std::make_unique<SystemSubclass>(),
				SystemSubclass::sign,
				std::unordered_set<EntityId>()
			)
		);
		// Someone claims I should do it like below because there is no implicit conversion for the above...
		// If I notice something going wrong keep it in mind
		//registeredSystems.push_back(std::unique_ptr<System>(new SystemSubclass()));
		return true;
	}

  // EntityId 0 means no entity
	EntityId createEntity()
	{
		// account for possible overflow into an already existing entity. Probably overkill.
		while ((entities.find(nextEntityId) != entities.end()) || (nextEntityId == 0)) nextEntityId++;

		// Create empty entity
		entities.insert({ nextEntityId, (Signature)0 });

		return nextEntityId++;
	}

	// Remember to check for nullptr (if I must)!
	template <typename ComponentType>
	std::shared_ptr<ComponentType> addComponent(EntityId id)
	{
		// Make sure I can only add registered component
		const ComponentId maxcid{ sizeof...(ComponentTypes) };
		const ComponentId cid{ getCompID<ComponentType>() };
		if (cid >= maxcid) return nullptr; // for now. See if I should throw

		auto foundEnt{ entities.find(id) };

		if (foundEnt == entities.end()) return nullptr;

		// Here I might want to check if component is currently being
		// removed and stop that from happening if I am adding.

		// Update signature of entities hashtable
		Signature prevSign{ foundEnt->second };
		foundEnt->second |= getSignature<ComponentType>();

		// If signature doesn't change component was already there
		if (prevSign == foundEnt->second) return nullptr;

		// Mark signature change so systems will know to update their entities.
		entSignUpdates[id] = foundEnt->second;

		auto compPtr = std::make_shared<ComponentType>();

		// If unique make sure there are no dupes
		if (std::is_base_of<UniqueComponent, ComponentType>::value)
			for (auto& dupeEntry : componentPools[cid])
				if (dupeEntry.first != id) removeComponent<ComponentType>(dupeEntry.first);

		componentPools[cid][id] = compPtr;

		// Unlike remove operation, component gets added immediately.
		//return std::static_pointer_cast<ComponentType>(compPtr);
		return compPtr;
	}

	// Operation will fully complete on the next tick!!!
	template <typename ComponentType>
	bool removeComponent(EntityId id)
	{
		// Make sure I can only remove registered component
		const ComponentId maxcid{ sizeof...(ComponentTypes) };
		const ComponentId cid{ getCompID<ComponentType>() };
		if (cid >= maxcid) return false; // for now. See if I should throw

		// Entity has already been deleted or doesn't exist.
		auto foundEnt{ entities.find(id) };
		if (foundEnt == entities.end()) return false;

		// Update signature of entities hashtable
		Signature prevSign{ foundEnt->second };
		foundEnt->second &= ~getSignature<ComponentType>();

		// If signature doesn't change component was not present
		if (prevSign == foundEnt->second) return false;

		// Mark signature change so systems will know to update their entities.
		entSignUpdates[id] = foundEnt->second;

		// Mark deleted components so they can be deleted after done iterating entities.
		deletedComponents[cid].insert(id);

		return true;
	}

	// Keep in mind that for one tick this can return a component from a deleted entity.
	template <typename ComponentType>
	std::shared_ptr<ComponentType> getComponent(EntityId id)
	{
    //// Make sure I can only add registered component
    //const ComponentId maxcid{ sizeof...(ComponentTypes) };
    //const ComponentId cid{ getCompID<ComponentType>() };
    //if (cid >= maxcid) return nullptr; // for now. See if I should throw

    //auto& pool = componentPools[cid];
    
    auto& pool = getComponentPool<ComponentType>();

		auto found = pool.find(id);

		if (found == pool.end()) return nullptr;

		return std::static_pointer_cast<ComponentType>(found->second);
	}

  // Keep in mind that for one tick this can return a component from a deleted entity.
  template <typename ComponentType>
  const ComponentPool& getComponentPool()
  {
    // Make sure I can only add registered component
    const ComponentId maxcid{ sizeof...(ComponentTypes) };
    const ComponentId cid{ getCompID<ComponentType>() };
    if (cid >= maxcid) throw std::runtime_error("Component not registered.");

    return componentPools[cid];
  }

	// Entity gets deleted immediately from entities, but components will go away next tick.
	// All Systems will run on this entity one last time!
	bool deleteEntity(EntityId id)
	{
		// Careful! If a component holds an entity id that gets deleted and then another
		// entity gets the id weird things might happen. Seems unlikely but keep in mind.

		// Delete all components of entity.
		// Do this before deleting entity
		// because removeComponent doesn't
		// work on non existent components.
		((removeComponent<ComponentTypes>(id)), ...);

		if (entities.erase(id) == 0) return false;

    // Invalidate target of connected entities
    std::vector<EntityConnection*> removedConnections;
    auto range = entityConnections.equal_range(id);
    for (auto it = range.first; it != range.second; ++it) {
      removedConnections.push_back(it->second);
    }
    for (auto entConPtr : removedConnections) entConPtr->setId((EntityId)0);
    removedConnections.clear();

		// Delete from componentPools and somehow run onDelete(?)
		// To prevent the danger above always make entity connections
		// two way, and when one gets deleted, it tells the other

		return true;
	}

private:
	// Remember the systems, signatures of registered systems and each system's entities
	std::vector<
		std::tuple<
			std::unique_ptr<System>,
			Signature,
			std::unordered_set<EntityId>
		>
	> registeredSystems{};

	int regSysNum{ 0 };
	EntityId nextEntityId{ 0 };

	ComponentId currentComponentID{ 0 };

	// array index is component id
	std::array<ComponentPool, sizeof...(ComponentTypes)> componentPools{};

	// Remember entity signature for fast available component checks
  std::unordered_map<EntityId, Signature> entities{};

	std::unordered_map<EntityId, Signature> entSignUpdates{};

	std::unordered_map<ComponentId, std::unordered_set<EntityId>> deletedComponents{};

	template <typename ComponentType>
	ComponentId getCompID()
	{
		static ComponentId myID = currentComponentID++;
		return myID;
	}

	// Do some stuff that must be done after I'm done
	// iterating entities to avoid segmentation faults
	void refreshSysEnts()
	{
		// Insert or Remove entity from registeredSystems based on its component signature
		for (auto& entry : entSignUpdates)
		{
			for (auto& sysTuple : registeredSystems)
			{
				if ((entry.second | std::get<1>(sysTuple)) == entry.second) std::get<2>(sysTuple).insert(entry.first);
				else std::get<2>(sysTuple).erase(entry.first);
			}
		}
		entSignUpdates.clear();

		// Attempt to erase all deleted components
		for (auto& dc : deletedComponents)
		{
			for (auto& entId : dc.second)
			{
				componentPools[dc.first].erase(entId);
			}
		}
		deletedComponents.clear();
	}
};

#endif
