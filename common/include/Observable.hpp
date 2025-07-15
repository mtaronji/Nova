#pragma once

#include <vector>
#include <functional>
#include <memory>
#include <optional>


template<typename T>
using Observer = std::function<void(const T&)>; 

template<typename T>
class Observable {
public:
	
	void Subscribe(Observer<T> observer) {
		observers.push_back(observer);
	}

	std::shared_ptr<Observable<T>> MapPrevious();
	std::shared_ptr<Observable<T>> MapDelta();
	
	template<typename MappedType>
	std::shared_ptr<Observable<MappedType>> Map(std::function<MappedType(const T&)>);
	
protected:
	std::vector<Observer<T>> observers;
};

template<typename T>
class Signal : public Observable<T> {
public:
	virtual void Emit(const T& value) {
		for (const Observer<T>& observer : this->observers) {
			observer(value);
		}
	}
protected:

};
/// <summary>
/// Map Previous Operator
/// This operator returns the previous value from the emission stream then the current
/// </summary>
/// <typeparam name="T"></typeparam>
/// <param name="observer"></param>
/// <returns></returns>
template<typename T>
std::shared_ptr<Observable<T>> Observable<T>::MapPrevious() {

	auto previousObservable = std::make_shared<Signal<T>>();
	std::optional<T> previousValue;

	this->Subscribe([previousObservable, previousValue](const T& emission) {

		if (previousValue.has_value()) {
			previousObservable->Emit(previousValue);
		}
		*previousValue = emission;
	});

	return previousObservable;
}

/// <summary>
/// Map Delta Operator
/// This operator returns the difference between the current emission and the last emission
/// any T that uses it must have the subtraction operator overloaded
/// </summary>
/// <typeparam name="T"></typeparam>
/// <param name="observer"></param>
/// <returns></returns>
template<typename T>
std::shared_ptr<Observable<T>> Observable<T>::MapDelta() {

	auto deltaObservable = std::make_shared<Signal<T>>();
	std::optional<T> previousValue;

	this->Subscribe([deltaObservable, previousValue](const T& emission) mutable {

		if (previousValue.has_value()) {
			deltaObservable->Emit(emission - *previousValue);
		}
		previousValue = emission;
		
	});

	return deltaObservable;
}

template<typename T>
template<typename MappedType>
std::shared_ptr<Observable<MappedType>> Observable<T>::Map(std::function<MappedType(const T&)> map) {

	auto mappedObservable = std::make_shared<Signal<MappedType>>();

	this->Subscribe([mappedObservable, map](const T& emission) {		
		mappedObservable->Emit(map(emission));	
	});
	return mappedObservable;
}
