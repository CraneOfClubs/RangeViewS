#pragma once
#include <vector>
#include <functional>
#include <iostream>

namespace view {

	template<typename T>
	class RangeView {
	public:
		typedef std::function< std::vector<T>(std::vector<T>&) > VFunc;
		typedef std::vector<VFunc> VFuncs;

		RangeView(std::function< std::vector<T>(std::vector<T>&) > func) 
		{
			actions.push_back(func);
		}

		template<class T>
		friend RangeView<T> operator|(std::vector<T> &v, RangeView<T> rv);

		template<typename T, typename F>
		friend std::vector<F> operator|(RangeView<T> rv, std::function< std::vector<F>(std::vector<T>&) > _predicate);

	private:
		void setCollection(std::vector<T> *_v) 
		{
			storage = _v;
		}

		std::vector<T> &getCollection() 
		{
			return *storage;
		}

		VFuncs & getActions() 
		{
			return actions;
		}
		std::vector<T> *storage;
		VFuncs actions;

	};

	template<typename T>
	RangeView<T> operator|(std::vector<T> &vec, RangeView<T> rv) 
	{
		rv.setCollection(&vec); //
		return rv;
	}

	template<typename T, typename F>
	std::vector<F> operator|(RangeView<T> rv, std::function< std::vector<F>(std::vector<T>&) > _predicate) 
	{
		std::vector<T> &result = rv.getCollection();
		for (auto &act : rv.getActions()) 
		{
			result = act(result);

		}
		return _predicate(result);

	}
}