#pragma once

#include <artemis/Config.h>

namespace artemis {

	class Component {
		public:
			virtual ~Component() = 0;
		protected:
			Component() {};
	};

}