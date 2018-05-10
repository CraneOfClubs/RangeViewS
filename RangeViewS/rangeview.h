#pragma once
#include <vector>
#include <functional>
#include <iostream>
#include <numeric>

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

		RangeView(std::function< std::vector<T>(std::vector<T>&) > func) 
		{
			_actions.push_back(func);
		}
		RangeView(std::vector<T> &v) : _storage(v) {}
		RangeView(VFunc generator) : _storage(std::vector<T>()), _generator(generator), _constructed(true), _infinte(true) {}
		RangeView() : _storage(std::vector<T>()) {}

		template<class T>
		friend RangeView<T> operator|(std::vector<T> &v, RangeView<T> rv);

		template<typename T, typename F>
		friend std::vector<F> operator|(RangeView<T> rv, std::function< std::vector<F>(std::vector<T>&) > _predicate);

		std::vector<T> toVector();

		template<typename T>
		operator std::vector<T>() { return toVector(); }

		void setCount(int to) 
		{
			_infinte = false;
			_counterInit = true;
			_count = to;
		}

		bool isCounterInited() 
		{
			return _counterInit;
		}

		int Count() 
		{
			return _count;
		}

		void replaceStorage(const std::vector<T> &storage) 
		{
			_storage.clear();
			_storage = storage;
		}

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
		bool _counterInit = false;
		int _count = 0;

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
		res.add(std::function< std::vector<T>(std::vector<T>&, RangeView<T>&) >(func.function));
		return res;
	}

	template<typename T, typename F>
	RangeView<T> operator|(std::vector<T> &vect, LazyFiniteInvoker<F> func) 
	{
		RangeView<T> res = RangeView<T>(vect);
		res.add(std::function< std::vector<T>(std::vector<T>&, RangeView<T>&) >(func.function));
		return res;
	}

	template<typename T, typename F>
	RangeView<T> operator|(RangeView<T> range, Invoker<F> func) {
		range.add(std::function< std::vector<T>(std::vector<T>&, RangeView<T>&) >(func.function));
		return range;
	}

	template<typename T, typename F>
	auto operator|(RangeView<T> range, LazyFiniteInvoker<F> invoker)
	{
		if (range.isInfinite())
		{
			invoker(std::vector<T>(), range);
		}
		else 
		{
			range.add(std::function< std::vector<T>(std::vector<T>&, RangeView<T>&) >(invoker.function));
		}
		return range;
	}

	template<typename T, typename Res>
	struct TwoPack 
	{
		TwoPack(RangeView<T> oldr, RangeView<Res> newr, std::function< std::vector<Res>(std::vector<T>&, RangeView<T>&) >func) : firstRange(oldr), newRange(newr), _func(func) {}
		RangeView<T> firstRange;

		RangeView<Res> newRange;
		std::function<std::vector<Res>(std::vector<T>&, RangeView<T>&)> _func;
	};

	template<typename T, typename F>
	auto operator|(RangeView<T> rv, FiniteInvoker<F> invoker) 
	{
		auto __storage = invoker(std::vector<T>(), rv);
		using type = std::decay_t<decltype(__storage[0])>;
		TwoPack<T, type> pack = TwoPack<T, type>(rv, RangeView<type>(), std::function< std::vector<type>(std::vector<T>&, RangeView<T>&) >(invoker.function));
		return pack;
	}


	template<typename T, typename ST, typename F>
	auto operator|(TwoPack<T, ST> pack, LazyFiniteInvoker<F> invoker) 
	{
		if (pack.firstRange.isCounterInited() == false)
		{
			invoker(std::vector<T>(), pack.firstRange);
		}

		pack.newRange.add(std::function< std::vector<ST>(std::vector<ST>&, RangeView<ST>&)>(invoker.function));
		auto _tc = static_cast<std::vector<T>>(pack.firstRange);
		pack.newRange.replaceStorage(pack._func(_tc, pack.firstRange));
		return pack.newRange;
	}

	template<typename T, typename R, typename F>
	auto operator|(TwoPack<T, R> pack, Invoker<F> invoker) 
	{
		pack.newRange.add(std::function< std::vector<R>(std::vector<R>&, RangeView<R>&) >(invoker.function));
		return pack;
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

	RangeView<int> ints(int n) 
	{
		auto int_generator_func = [n](auto &vec, auto &other) 
		{
			for (int i = n, _count = 0; _count < other.Count(); i++, _count++) 
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


	auto take(uint32_t n) 
	{
		auto take_func = [n](auto &v, auto &rv)
		{
			rv.setCount(n);
			if (!rv.isInfinite())
			{
				if (n < rv.getResult().size())
					rv.getResult().resize(n);
			}
			return v;
		};
		return LazyFiniteInvoker<decltype(take_func)>(take_func);
	}

	template<typename F>
	auto transform(F &&pred)
	{
		auto transform_func = [pred](auto &v, auto &rv) 
		{
			std::vector<std::decay_t<decltype(pred(v[0]))>> result;
			for (size_t i = 0; i < v.size(); i++) 
			{
				result.push_back(pred(v[i]));
			}

			return result;
		};
		return FiniteInvoker<decltype(transform_func)>(transform_func);
	}

	template <class TView>
	auto accumulate(TView view) 
	{
		auto collection = view.toVector();
		using type = std::decay_t<decltype(collection[0])>;
		return std::accumulate(collection.begin(), collection.end(), type());
	}

	template<typename F>
	auto remove_if(F &&func) 
	{
		auto remove_if_func = [func](auto &v, auto &rv) 
		{
			for (size_t i = 0; i < v.size();) 
			{
				if (func(v[i]))
					v.erase(v.begin() + i);
				else
					i++;
			}
			return v;
		};

		return Invoker<decltype(remove_if_func)>(remove_if_func);
	}

	auto reverse() 
	{
		auto func = [](auto &v, auto &rv) 
		{
			std::reverse(v.begin(), v.end());
			return v;
		};

		return Invoker<decltype(func)>(func);
	}
}