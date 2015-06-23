// IInitializable.hpp

#ifndef __IINITIALIZABLE_HPP__
#define __IINITIALIZABLE_HPP__

class IInitializable
{
	public:
		IInitializable() = default;
		virtual ~IInitializable() = default;

		virtual bool Initialize() = 0;
		virtual bool Finalize() = 0;
};

#endif
