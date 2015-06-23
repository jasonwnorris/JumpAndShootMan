// ISingleton.hpp

#ifndef __ISINGLETON_HPP__
#define __ISINGLETON_HPP__

template<typename T> class ISingleton
{
	public:
		static T& GetInstance()
		{
			static T instance;

			return instance;
		}

	private:
		ISingleton() = default;
		~ISingleton() = default;
		ISingleton(const ISingleton& p_Other) = delete;
		void operator=(const ISingleton& p_Other) = delete;
};

#endif
