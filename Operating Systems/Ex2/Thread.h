/**
 * @file Temp_Thread.h
 *
 * @section DESCRIPTION
 * A class representing a Thread.
 */

#ifndef OPERATING_SYSTEMS_THREAD_H
#define OPERATING_SYSTEMS_THREAD_H

// ------------------------------ Includes ------------------------------

#include <iostream>
#include <stdio.h>
#include <setjmp.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>

// ------------------- SP, PC and entry point function ------------------
#define JB_SP 6
#define JB_PC 7
typedef void (*entry)();

class Thread
{
public:
	/**
	 * Default Constructor.
	 */
	Thread();
	
	/**
	 * A constructor for Thread.
	 * @param id - int representing the Thread's ID
	 * @param priority - according to given priority array
	 * @param stackSize
	 * @param f - the function of the thread
	 */
	Thread(int id, int priority, int stackSize, entry f);
	
	/**
	 * Copy constructor.
	 */
	Thread(const Thread &other);
	
	/**
	 * Operator overloading for =.
	 */
	Thread &operator=(const Thread &other);
	
	/**
	 * Destructor.
	 */
	~Thread();
	
	/**
	 * @return - The ID of the thread.
	 */
	int getId() const;
	
	/**
	 * @return - The state of the thread.
	 */
	bool getBlocked() const;
	
	/**
	 * Sets the threads state to be new state.
	 */
	void setBlocked(bool newS);
	
	/**
	 * @return - The amount of times the thread was in RUNNING mode.
	 */
	int getIteration() const;
	
	/**
	 * Increment the quantums amount by 1.
	 */
	void incIteration();
	
	/**
	 * @return priority of thread
	 */
	int getPriority();
	
	/**
	 * Sets the threads priority to be new priority.
	 */
	void setPriority(int priority);
	
	/**
	 * @return - The sigjmp buffer of the thread.
	 */
	sigjmp_buf &getSjb();
	
private:
	int _id;
	int _iteration;
	bool _blocked;
	int _priority;
	int _stackSize;
	char *_stack;
	sigjmp_buf _sjb;
	entry _f;
};


#endif //OS_EX2_TEMP_THREAD_H