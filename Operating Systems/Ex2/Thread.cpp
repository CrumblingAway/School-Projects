/**
 * @file Thread.cpp
 *
 * @section DESCRIPTION
 * Implementation of Thread class.
 */

// ------------------------------ Includes ------------------------------
#include "Thread.h"

// --------------------------- Address typedef --------------------------
typedef unsigned long address_t;


// ------------------------------ Functions -----------------------------
/* A translation is required when using an address of a variable.
   Use this as a black box in your code. */
address_t translate_address(address_t addr)
{
    address_t ret;
    asm volatile("xor    %%fs:0x30,%0\n"
                 "rol    $0x11,%0\n"
    : "=g" (ret)
    : "0" (addr));
    return ret;
}

Thread::Thread():
		_id(-1), _iteration(0), _blocked(-2), _priority(-1), _stackSize(0), _stack(nullptr), _sjb(),
		_f(nullptr)
{
}

Thread::Thread(int id, int priority, int stackSize, entry f):
		_id(id), _iteration(0), _blocked(false), _priority(priority), _stackSize(stackSize), _stack(new char[(id==0) ? 0 : stackSize]),
		_sjb(), _f(f)
{
	address_t sp, pc;
	sp = (address_t) _stack + stackSize - sizeof(address_t);
	pc = (address_t) _f;
	sigsetjmp(_sjb, 1);
	(_sjb->__jmpbuf)[JB_SP] = translate_address(sp);
	(_sjb->__jmpbuf)[JB_PC] = translate_address(pc);
	sigemptyset(&_sjb->__saved_mask);
}

Thread::Thread(const Thread &other):
        Thread(other._id, other._priority, other._stackSize, other._f)
{
	_blocked = other._blocked;
}

Thread &Thread::operator=(const Thread &other)
{
    if (&other == this)
    {
        return *this;
    }
    _id = other._id;
	_blocked = other._blocked;
    _priority = other._priority;
    delete[] _stack;
    _stack = nullptr;
    _stackSize = other._stackSize;
    _stack = new char[other._stackSize];
    for (int i = 0; i < _stackSize; i++)
    {
        _stack[i] = other._stack[i];
    }
    _sjb[0] = other._sjb[0];
    return *this;
}

Thread::~Thread()
{
    delete[] _stack;
}

int Thread::getId() const
{
    return _id;
}

bool Thread::getBlocked() const
{
    return _blocked;
}

void Thread::setBlocked(bool newS)
{
	_blocked = newS;
}

int Thread::getIteration() const
{
    return _iteration;
}

void Thread::incIteration()
{
    _iteration++;
}

int Thread::getPriority()
{
	return _priority;
}

void Thread::setPriority(int priority)
{
    _priority = priority;
}

sigjmp_buf &Thread::getSjb()
{
    return _sjb;
}


	
