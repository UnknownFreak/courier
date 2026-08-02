#include "run.h"
/*
#include "courier/courier.hpp"
#include "courier/logger.hpp"
#include "courier/messageHandler.hpp"
#include "courier/util.hpp"
#include "sample_generic/object.h"
#include <iostream>



namespace sample::simulation
{
    size_t g_id = 1;

    namespace events
    {
        enum class stashType
        {
            wood, iron, gold, spawner
        };

        struct workEvent
        {
            stashType type;
            size_t count;
        };
        
        struct stashEvent
        {
            stashType type;
            size_t count;
        };
        struct createObject
        {
            stashType type;
        };
    }

    struct forest
    {
        size_t wood = 1000000000;
    };

    struct mine
    {
        size_t iron = 1000000000;
        size_t gold = 1000000000;
    };

    struct worker
    {
        courier::SubscriberId id;
        events::stashType workerType;
        float resetWorkTime;
        float workTime;
        courier::SubscriberId getId() const {return id;}
    };

    struct stash
    {
        size_t wood = 0;
        size_t iron = 0;
        size_t gold = 0;
        float every = 5.f;
    };


    struct Collection
    {
        std::vector<generic::object> objects;
        std::vector<worker> workers;

        forest m_forest;
        mine m_mine;
        stash m_stash;
        
        template<class Type>
		void onMessage(const Type& message)
		{
			courier::handleMessage(objects, message);
			courier::handleMessage(workers, message);

            courier::handleObjectMessage(m_forest, message);
            courier::handleObjectMessage(m_mine, message);
            courier::handleObjectMessage(m_stash, message);
            courier::handleEvent(message);
		}

        template<class Type>
		void onMessage(courier::SubscriberId id, const Type& message)
		{
			courier::handleMessage(objects, id, message);
			courier::handleMessage(workers, id, message);
		}

        void remove(const courier::SubscriberId id)
        {
            std::vector<courier::SubscriberId> vec = {id};
            courier::util::vectorFastRemove(objects, vec, false);
            courier::util::vectorFastRemove(workers,vec, false);
        }

        bool hasObjects() const
        {
            bool b =  objects.size() > 0;
            if(b == false)
            {
                std::cout << "No more objects" << std::endl;
            }
            return b;
        }
    };

    courier::Courier<Collection>* ptr = nullptr;
};

namespace courier
{
    

    template<>
	void handleObjectMessage(sample::simulation::worker& obj, const sample::generic::deltaTime&time)
	{
        obj.workTime-= time.dt;
        if(obj.workTime < 0)
        {
            using namespace sample::simulation::events;
            if(obj.workerType == stashType::spawner)
                sample::simulation::ptr->schedule(sample::simulation::events::createObject{stashType::gold});
            else
                sample::simulation::ptr->schedule(workEvent{obj.workerType, 5});
            obj.workTime = obj.resetWorkTime;
        }
	}

    template<>
	void handleObjectMessage(sample::simulation::mine& obj, const sample::simulation::events::workEvent& event)
	{
            using namespace sample::simulation::events;
        if(event.type == stashType::iron)
        {
            if(obj.iron < event.count)
            {
                if(obj.iron != 0)
                {
                    sample::simulation::ptr->schedule(stashEvent{stashType::iron, obj.iron});
                }
                obj.iron = 0;
            }
            else
            {
                sample::simulation::ptr->schedule(stashEvent{stashType::iron, event.count});
                obj.iron-= event.count;
            }
        }
        else if(event.type == stashType::gold)
        {
            if(obj.gold < event.count)
            {
                if(obj.gold != 0)
                {
                    sample::simulation::ptr->schedule(stashEvent{stashType::gold, obj.gold});
                }
                obj.gold = 0;
            }
            else
            {
                sample::simulation::ptr->schedule(stashEvent{stashType::gold, event.count});
                obj.gold-= event.count;
            }
        }
	}

    template<>
	void handleObjectMessage(sample::simulation::forest& obj, const sample::simulation::events::workEvent& event)
	{
            using namespace sample::simulation::events;
        if(event.type == stashType::wood)
        {
            if(obj.wood < event.count)
            {
                if(obj.wood != 0)
                {
                    sample::simulation::ptr->schedule(stashEvent{stashType::wood, obj.wood});
                }
                obj.wood = 0;
            }
            else
            {
                sample::simulation::ptr->schedule(stashEvent{stashType::wood, event.count});
                obj.wood-= event.count;
            }
        }
	}

    template<>
	void handleObjectMessage(sample::simulation::stash& obj, const sample::simulation::events::stashEvent& event)
	{
        using namespace sample::simulation::events;
        if(event.type == stashType::iron)
        {
            obj.iron+= event.count;
        }
        else if(event.type == stashType::gold)
        {
            obj.gold+= event.count;
        }
        else if(event.type == stashType::wood)
        {
            obj.wood+= event.count;
        }
	}
        template<>
	void handleObjectMessage(sample::simulation::stash& obj, const sample::generic::deltaTime& event)
	{
        using namespace sample::simulation::events;
        obj.every-= event.dt;
        if(obj.every < 0)
        {
            obj.every = 5.f;
            std::cout << "Wood=" << obj.wood << std::endl;
            std::cout << "Iron=" << obj.iron << std::endl;
            std::cout << "Gold=" << obj.gold << std::endl;
        }
	}

}
*/

namespace sample::simulation
{
    /*
    class ConsoleLogger : public courier::Logger
	{
	public:
		virtual void info(const std::string_view view)
        {
            std::cout << "[INFO] " << view << std::endl;
        }
		virtual void warning(const std::string_view view)
        {
            std::cout << "[WARN] " << view << std::endl;
        };
		virtual void error(const std::string_view view)
        {
            std::cout << "[ERR ] " << view << std::endl;
        }

	};

    void addObject(auto& coll, events::stashType type)
    {
        courier::SubscriberId id = (courier::SubscriberId)g_id++;
        coll.objects.emplace_back(id);
        coll.workers.emplace_back(id, type, 0.1f, 0.f);
    }

    void setup(auto& instance)
    {
        Collection& coll = instance.getCollection();
        addObject(coll, events::stashType::wood);
        addObject(coll, events::stashType::wood);
        addObject(coll, events::stashType::iron);
        addObject(coll, events::stashType::gold);
        addObject(coll, events::stashType::spawner);
    };

    void runTest(auto& instance)
    {
		auto start = std::chrono::high_resolution_clock::now();

        while (true)
		{
			auto end = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double, std::milli> elapsed = end - start;
            
            if(elapsed.count() >= (1/60.f*1000))
            {
                start = std::chrono::high_resolution_clock::now();
                instance.post(generic::deltaTime{1/60.f});
                instance.endFrame();
            }
        }
    }
    */

    void run()
    {
        /*
        courier::setLogger(std::make_shared<ConsoleLogger>());
        {
			courier::Courier<Collection> instance(courier::Settings{ courier::ThreadingSettings::Fixed, 1 });
            ptr = &instance;
            setup(instance);
            runTest(instance);
		}
            */
    }
}
/*
namespace courier
{
    template<>
	void handleEvent(const sample::simulation::events::createObject& createObject)
	{
        (void)createObject;
        sample::simulation::ptr->scheduleFunction([createObject](){sample::simulation::addObject(sample::simulation::ptr->getCollection(), createObject.type);});
	}
}
    */