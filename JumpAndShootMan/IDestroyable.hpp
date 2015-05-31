// IDestroyable.hpp

#ifndef __IDESTROYABLE_HPP__
#define __IDESTROYABLE_HPP__

class IDestroyable
{
	public:
		IDestroyable() { m_IsMarkedForDestruction = false; }
		~IDestroyable() { }

		virtual bool IsAlive() const { return !m_IsMarkedForDestruction; }
		virtual void Destroy() { m_IsMarkedForDestruction = true; }

	private:
		bool m_IsMarkedForDestruction;
};

#endif
