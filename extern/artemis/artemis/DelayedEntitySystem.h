#pragma once

#include <artemis/Config.h>
#include <artemis/EntitySystem.h>

namespace artemis {
	class DelayedEntitySystem : public EntitySystem {
			float _delay;
			float _acc;
			bool _running;
			bool _autoRepeat;

		protected:
			virtual void processEntities(const std::vector<std::shared_ptr<Entity>>& entities);
			virtual void processEntities(const std::vector<std::shared_ptr<Entity> >& entities, float accumulatedDelta) = 0;
			virtual bool checkProcessing();

		public:

			DelayedEntitySystem(const Aspect& aspect);

			void start(float delay);
			void stop();
			void setAutoRepeat(bool repeat);
			float getInitialTimeDelay() const;
			float getRemainingTime() const;
			bool isRunning() const;
	};
}
