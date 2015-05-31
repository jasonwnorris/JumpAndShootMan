// IComponent.hpp

#ifndef __ICOMPONENT_HPP__
#define __ICOMPONENT_HPP__

// Project Includes
#include "IDestroyable.hpp"
// STL Includes
#include <string>

class Entity;

class IComponent : public IDestroyable
{
	public:
		IComponent(Entity* p_Owner);
		virtual ~IComponent();

		Entity* GetOwner() const;
		const std::string& GetName() const;

		virtual bool CanUpdate() const;
		virtual bool CanRender() const;

	protected:
		Entity* m_Owner;
		std::string m_Name;
};

#endif
