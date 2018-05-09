#pragma once
#include <vector>
#include <functional>
#include <iostream>

namespace view {

	template<typename T>
	class RangeView;

	template <typename F>
	class Invoker
	{
	public:
		Invoker(F &function) : function(function) {}

		template <typename T>
		auto operator()(const std::vector<T> &v, RangeView<T> &rv)
		{
			return function(v, rv);
		}
		F function;
	};

	template<typename F>
	struct FiniteInvoker : Invoker<F>
	{
		FiniteInvoker(F &function) : Invoker<F>(function) {}
	};

	template<typename F>
	struct LazyFiniteInvoker : Invoker<F>
	{
		LazyFiniteInvoker(F &function) : Invoker<F>(function) {}
	};

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
		std::vector<T> toVector();

		bool isInfinite()
		{
			return _infinte;
		}

		std::vector<T>& getResult() 
		{
			return _result;
		}

		void add(VFunc func)
		{
			_actions.push_back(func);
		}

	private:
		VFunc _generator;
		bool _infinte = false;
		bool _constructed = false;
		std::vector<T> _storage;
		std::vector<T> _result;
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


	template<typename T, typename F>
	RangeView<T> operator|(std::vector<T> &vect, Invoker<F> func) 
	{
		RangeView<T> res = RangeView<T>(vect);
		res.addAction(std::function< std::vector<T>(std::vector<T>&, RangeView<T>&) >(func.function));
		return res;
	}

	template<typename T, typename F>
	RangeView<T> operator|(std::vector<T> &vect, LazyFiniteInvoker<F> func) 
	{
		RangeView<T> res = RangeView<T>(vect);
		res.addAction(std::function< std::vector<T>(std::vector<T>&, RangeView<T>&) >(func.function));
		return res;
	}

	template<typename T, typename F>
	auto operator|(RangeView<T> range, LazyFiniteInvoker<F> termOp)
	{
		if (range.isInfinite())
		{
			termOp(std::vector<T>(), range);
		}
		else 
		{
			range.add(std::function< std::vector<T>(std::vector<T>&, RangeView<T>&) >(termOp.function));
		}
		return range;
	}




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

	template<typename T>
	std::vector<T> RangeView<T>::toVector()
	{
		if (_constructed)
		{
			if (_infinte)
			{
				throw new std::bad_cast;
			}
			_generator(_result, *this);
		}
		else
		{
			_result.clear();
			_result.assign(this->getCollection().begin(), this->getCollection().end());
		}

		for (auto &action : this->getActions())
		{
			action(_result, *this);
		}
		return _result;
	}



	auto take(int n) {
		auto take_func = [n](auto &v, auto &rv) {
			rv.Count = n;
			if (!rv.isInfinite())
			{
				if (n < rv.getResult().size()) {
					rv.getResult().resize(n);
				}
			}

			return v;
		};

		return LazyFiniteInvoker<decltype(take_func)>(take_func);
	}
}