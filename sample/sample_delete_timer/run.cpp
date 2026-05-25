#include "run.h"
#include "courier/messageHandler.hpp"

#include "courier/subscriberId.hpp"
#include "courier/util.hpp"

#include <cstddef>
#include <memory>
#include <sample_generic/object.h>

#include <courier/courier.hpp>
#include <courier/logger.hpp>

#include <iostream>


namespace sample::timer
{
    size_t g_id = 1;
    

    struct delete_timer
    {
        courier::SubscriberId id;
        float remainingTime;
        courier::SubscriberId getId() const { return id;}
    };

    struct Collection
    {
        std::vector<generic::object> objects;
        std::vector<delete_timer> deleteTimers;
        template<class Type>
		void onMessage(const Type& message)
		{
			courier::handleMessage(objects, message);
			courier::handleMessage(deleteTimers, message);
		}

        template<class Type>
		void onMessage(courier::SubscriberId id, const Type& message)
		{
			courier::handleMessage(objects, id, message);
			courier::handleMessage(deleteTimers, id, message);
		}

        void remove(std::vector<courier::SubscriberId>& vec)
        {
            if(vec.size() != 0)
            {
                std::cout << "Removing ["<< vec.size() << "] item(s) from container [size="<< objects.size() << "]" << std::endl;
            }
            courier::util::vectorFastRemove(objects,vec, false);
            courier::util::vectorFastRemove(deleteTimers,vec, true);
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
    /*
    template<>
	void handleMessage<sample::generic::object, sample::generic::deltaTime>(std::vector<sample::generic::object>&, const sample::generic::deltaTime&)
	{
		static bool once = true;
		if(once)
		{
			std::cout << "deltatime messages ignored for object" << std::endl;
			once = false;
		}
		return;
	}

    template<>
	void handleMessage<sample::timer::delete_timer, sample::generic::deleteObject>(std::vector<sample::timer::delete_timer>&, const sample::generic::deleteObject&)
	{
		static bool once = true;
		if(once)
		{
			std::cout << "deleteObject messages ignored for delete_timer" << std::endl;
			once = false;
		}
		return;
	}

    template<>
	void handleMessage<sample::timer::delete_timer, sample::generic::deleteObject>(std::vector<sample::timer::delete_timer>&, SubscriberId, const sample::generic::deleteObject&)
	{
		static bool once = true;
		if(once)
		{
			std::cout << "deleteObject messages ignored for delete_timer" << std::endl;
			once = false;
		}
		return;
	}*/

    template<>
	void handleObjectMessage(sample::timer::delete_timer& timer, const sample::generic::deltaTime&time)
	{
        timer.remainingTime-= time.dt;
        if(timer.remainingTime < 0)
        {
            std::cout << "Requesting to delete object id[" << std::to_string((size_t)timer.id) <<"]" << std::endl;
            sample::timer::ptr->schedule(timer.id, sample::generic::deleteObject{});
        }
	}
    template<>
	void handleObjectMessage(sample::generic::object& obj, const sample::generic::deleteObject&)
	{
        std::cout << "Handling courier delete object request for id[" << std::to_string((size_t)obj.id) <<"]" << std::endl;
        sample::timer::ptr->remove(obj.id);
	}
}

namespace sample::timer
{

    void addObject(auto& coll, float timeUntilDestruction)
    {
        courier::SubscriberId id = (courier::SubscriberId)g_id++;
        std::cout << "object created with id ["<< (size_t)id << "] with timed life [duraction="<< timeUntilDestruction <<"s]" << std::endl;
        coll.objects.emplace_back(id);
        coll.deleteTimers.emplace_back(id, timeUntilDestruction);
    }

    void setup(auto& instance)
    {
        Collection& coll = instance.getCollection();
        addObject(coll, 1);
        addObject(coll, 1);
        addObject(coll, 1.5f);

    };

    void runTest(auto& instance)
    {
		auto start = std::chrono::high_resolution_clock::now();

        while (instance.getCollection().hasObjects())
		{
			auto end = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double, std::milli> elapsed = end - start;
            
            if(elapsed.count() >= (1/60.f*1000))
            {
                start = std::chrono::high_resolution_clock::now();
                instance.post(generic::deltaTime{1/60.f});
            }
        }
    }

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

    void run()
    {
        courier::setLogger(std::make_shared<ConsoleLogger>());
        {
			courier::Courier<Collection> instance(courier::Settings{ courier::ThreadingSettings::Fixed, 1 });
            ptr = &instance;
            setup(instance);
            runTest(instance);
		}
    }
}