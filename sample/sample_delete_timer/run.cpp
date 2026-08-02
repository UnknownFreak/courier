#include "run.h"
#include "courier/messageHandler.hpp"

#include "courier/objectId.hpp"
#include "courier/util.hpp"

#include <cstddef>
#include <memory>
#include <sample_generic/object.h>

#include <courier/courier.hpp>
#include <courier/logger.hpp>

#include <iostream>
#include <vector>


namespace sample::timer
{
    size_t g_id = 1;
    

    struct delete_timer
    {
        courier::ObjectId id;
        float remainingTime;
        courier::ObjectId getId() const { return id;}
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
		void onMessage(courier::ObjectId id, const Type& message)
		{
			courier::handleMessage(objects, id, message);
			courier::handleMessage(deleteTimers, id, message);
		}

        void remove(const courier::ObjectId id)
        {
            /*
            if(vec.size() != 0)
            {
                std::cout << "Removing ["<< vec.size() << "] item(s) from container [size="<< objects.size() << "]" << std::endl;
            }
            */
            std::vector<courier::ObjectId> vec = {id};
            courier::util::vectorFastRemove(objects,vec, false);
            //courier::util::vectorFastRemove(deleteTimers,vec, true);
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
	void handleObjectMessage(sample::timer::delete_timer& timer, const sample::generic::deltaTime&time)
	{
        timer.remainingTime-= time.dt;
        if(timer.remainingTime < 0)
        {
            std::cout << "Requesting to delete object id[" << timer.id <<"]" << std::endl;
            sample::timer::ptr->schedule(timer.id, sample::generic::deleteObject{});
        }
	}
    template<>
	void handleObjectMessage(sample::generic::object& obj, const sample::generic::deleteObject&)
	{
        std::cout << "Handling courier delete object request for id[" << obj.id <<"]" << std::endl;
        sample::timer::ptr->remove(obj.id.rawId());
	}
}

namespace sample::timer
{

    void addObject(auto& coll, float timeUntilDestruction)
    {
        courier::ObjectId id;
        std::cout << "object created with id ["<< id << "] with timed life [duraction="<< timeUntilDestruction <<"s]" << std::endl;
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
                instance.endFrame();
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