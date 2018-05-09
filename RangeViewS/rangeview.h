#pragma once
#include <vector>
#include <functional>
#include <iostream>

namespace view {

	template<typename T>
	class RangeView {
	public:
		typedef std::function< std::vector<T>(std::vector<T>&, RangeView<T>&) > VFunc;
		typedef std::vector<VFunc> VFuncs;
		int Count = -1;

		RangeView(std::function< std::vector<T>(std::vector<T>&) > func) 
		{
			_actions.push_back(func);
		}
		RangeView(std::vector<T> &v) : _storage(v) {}
		RangeView(VFunc generator) : _storage(std::vector<T>()), _generator(generator), _constructed(true), _infinte(true) {}
		template<class T>
		friend RangeView<T> operator|(std::vector<T> &v, RangeView<T> rv);

		template<typename T, typename F>
		friend std::vector<F> operator|(RangeView<T> rv, std::function< std::vector<F>(std::vector<T>&) > _predicate);

	private:
		VFunc _generator;
		bool _infinte = false;
		bool _constructed = false;
		std::vector<T> _storage;
		VFuncs _actions;

		void setCollection(std::vector<T> *_v) 
		{
			_storage = _v;
		}

		std::vector<T> &getCollection() 
		{
			return _storage;
		}

		VFuncs &getActions() 
		{
			return _actions;
		}

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

	RangeView<int> ints(int n) {
		auto int_generator_func = [n](auto &vec, auto &other) 
		{
			for (int i = n, Count = 0; Count < other.Count; i++, Count++) 
			{
				vec.push_back(i);
			}

			return vec;
		};

		RangeView<int> rv = RangeView<int>(std::function<std::vector<int>(std::vector<int>&, RangeView<int>&)>(int_generator_func));

		return rv;
	}
}