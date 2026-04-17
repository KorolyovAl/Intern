#include <cassert>
#include <iostream>
#include <utility>

#include "my_shared_ptr.h"
#include "my_weak_ptr.h"

using my_ptr::SharedPtr;
using my_ptr::WeakPtr;

// struct for tracking amount of alive and destroyed objects
struct LifetimeTracker {
    static int alive;
    static int destroyed;
    int value = 0;

    explicit LifetimeTracker(int v = 0)
        : value(v) {
        ++alive;
    }

    ~LifetimeTracker() {
        --alive;
        ++destroyed;
    }
};

int LifetimeTracker::alive = 0;
int LifetimeTracker::destroyed = 0;

void ResetLifetimeCounters() {
    LifetimeTracker::alive = 0;
    LifetimeTracker::destroyed = 0;
}

struct Node {
    static int destroyed;
    SharedPtr<Node> next;
    WeakPtr<Node> prev;

    ~Node() {
        ++destroyed;
    }
};

int Node::destroyed = 0;

struct Base {
    static int base_destroyed;

    ~Base() {
        ++base_destroyed;
    }
};

int Base::base_destroyed = 0;

struct Derived : Base {
    static int derived_destroyed;

    ~Derived() {
        ++derived_destroyed;
    }
};

int Derived::derived_destroyed = 0;

void ResetPolymorphicCounters() {
    Base::base_destroyed = 0;
    Derived::derived_destroyed = 0;
}

void TestPolymorphicDeletion() {
    ResetPolymorphicCounters();

    {
        SharedPtr<Base> ptr(new Derived());
    }

    assert(Base::base_destroyed == 1);
    assert(Derived::derived_destroyed == 1);
}

// проверяется, что объект уничтожится только 1 раз
void TestMemoryManagement() {
    ResetLifetimeCounters();

    {
        SharedPtr<LifetimeTracker> first(new LifetimeTracker(10));
        {
            SharedPtr<LifetimeTracker> second(first);
            WeakPtr<LifetimeTracker> weak(first);

            assert(first.use_count() == 2);
            assert(second.use_count() == 2);
            assert(weak.use_count() == 2);
            assert(LifetimeTracker::alive == 1);
            assert(LifetimeTracker::destroyed == 0);
        }

        assert(first.use_count() == 1);
        assert(LifetimeTracker::alive == 1);
        assert(LifetimeTracker::destroyed == 0);
    }

    assert(LifetimeTracker::alive == 0);
    assert(LifetimeTracker::destroyed == 1);
}

// проверка пустых указателей и базового функионала
void TestBasicFunctionality() {
    ResetLifetimeCounters();

    SharedPtr<int> empty_shared;
    assert(!empty_shared);
    assert(empty_shared.get() == nullptr);
    assert(empty_shared.use_count() == 0);

    WeakPtr<int> empty_weak;
    assert(empty_weak.expired());
    assert(empty_weak.use_count() == 0);
    assert(!empty_weak.lock());

    {
        SharedPtr<LifetimeTracker> shared(new LifetimeTracker(42));
        WeakPtr<LifetimeTracker> weak(shared);

        assert(shared);
        assert(shared.get() != nullptr);
        assert(shared.use_count() == 1);
        assert(weak.use_count() == 1);
        assert(!weak.expired());
        assert((*shared).value == 42);
        assert(shared->value == 42);

        SharedPtr<LifetimeTracker> locked = weak.lock();
        assert(locked);
        assert(locked->value == 42);
        assert(shared.use_count() == 2);
        assert(weak.use_count() == 2);
    }

    assert(LifetimeTracker::alive == 0);
    assert(LifetimeTracker::destroyed == 1);

    WeakPtr<LifetimeTracker> expired_weak;
    {
        SharedPtr<LifetimeTracker> shared(new LifetimeTracker(7));
        expired_weak = shared;
        assert(!expired_weak.expired());
    }

    assert(expired_weak.expired());
    assert(expired_weak.use_count() == 0);
    assert(!expired_weak.lock());
}

void TestCyclesBrokenWithWeakPtr() {
    Node::destroyed = 0;

    {
        SharedPtr<Node> first(new Node());
        SharedPtr<Node> second(new Node());

        first->next = second;
        second->prev = first;
    }

    assert(Node::destroyed == 2);
}

void TestSelfAssignment() {
    ResetLifetimeCounters();

    {
        SharedPtr<LifetimeTracker> shared(new LifetimeTracker(99));
        WeakPtr<LifetimeTracker> weak(shared);

        shared = shared;
        assert(shared);
        assert(shared->value == 99);
        assert(shared.use_count() == 1);

        weak = weak;
        assert(!weak.expired());
        assert(weak.use_count() == 1);
    }

    assert(LifetimeTracker::alive == 0);
    assert(LifetimeTracker::destroyed == 1);
}

void TestMoveOwnershipTransfer() {
    ResetLifetimeCounters();

    {
        SharedPtr<LifetimeTracker> source(new LifetimeTracker(123));
        assert(source.use_count() == 1);

        SharedPtr<LifetimeTracker> moved(std::move(source));
        assert(!source);
        assert(source.get() == nullptr);
        assert(source.use_count() == 0);
        assert(moved);
        assert(moved->value == 123);
        assert(moved.use_count() == 1);

        SharedPtr<LifetimeTracker> target;
        target = std::move(moved);
        assert(!moved);
        assert(moved.get() == nullptr);
        assert(moved.use_count() == 0);
        assert(target);
        assert(target->value == 123);
        assert(target.use_count() == 1);
    }

    assert(LifetimeTracker::alive == 0);
    assert(LifetimeTracker::destroyed == 1);
}

void TestWeakPtrMove() {
    ResetLifetimeCounters();

    {
        SharedPtr<LifetimeTracker> shared(new LifetimeTracker(10));
        WeakPtr<LifetimeTracker> weak(shared);

        WeakPtr<LifetimeTracker> moved(std::move(weak));

        assert(weak.use_count() == 0);
        assert(moved.use_count() == 1);
        assert(!moved.expired());

        SharedPtr<LifetimeTracker> locked = moved.lock();
        assert(locked);
        assert(locked->value == 10);
    }

    assert(LifetimeTracker::alive == 0);
    assert(LifetimeTracker::destroyed == 1);
}

int main() {
    TestPolymorphicDeletion();
    TestMemoryManagement();
    TestBasicFunctionality();
    TestCyclesBrokenWithWeakPtr();
    TestSelfAssignment();
    TestMoveOwnershipTransfer();
    TestWeakPtrMove();

    std::cout << "All tests passed\n";
    return 0;
}
