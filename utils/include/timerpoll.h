/* 
 * File:   timer_poll.h
 * Author: Administrator
 *
 * Created on 2013年7月4日, 下午3:05
 */

#ifndef TIMER_POLL_H
#define	TIMER_POLL_H
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/epoll.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/timerfd.h>
#include <unistd.h>
#include <pthread.h>
#include <map>

#define MAXFDS 128
#define EVENTS 100
class timer;
typedef int(*timer_callback)(timer *);//user callback
using namespace std;
class timer
{
private:
    int     timer_id;
    double  timer_internal;
    void * userdata;
    bool    repeat;//will the timer repeat or only once
    timer_callback cb;

public:

    timer() :  timer_id(0),timer_internal(0.0) , userdata(0), repeat(0), cb(0){};
    timer(double internal_value, int  (*callback)(timer * ptimer), void * data, int rep) : timer_internal(internal_value), userdata(data), repeat(rep), cb(callback)
    {
        timer_id = timerfd_create(CLOCK_REALTIME, 0);
        setNonBlock(timer_id);
    };

    timer(const timer &ptimer);
    timer & operator=(const timer &ptimer);
    int timer_start();
    int timer_stop();
    int timer_modify_internal(double timer_internal);

    int timer_get_id()
    {
        return timer_id;
    }

    double timer_get_timer_interval(){
    	return timer_internal;
    }

    void * timer_get_userdata()
    {
        return userdata;
    }

    timer_callback get_user_callback()
    {
        return cb;
    }

    ~timer()
    {
        timer_stop();
    }

private:

    bool setNonBlock (int fd)
    {
        int flags = fcntl (fd, F_GETFL, 0);
        flags |= O_NONBLOCK;
        if (-1 == fcntl (fd, F_SETFL, flags))
        {
            return false;
        }
        return true;
    }

} ;

class timers_poll
{
public:
    timers_poll(int max_num=128)
    {
        active = 1;
        epfd = epoll_create(max_num);
    }

    int timers_poll_add_timer(timer * ptimer);
    int timers_poll_del_timer(timer * ptimer);
    int run();

    int timers_poll_deactive()
    {
        active = 0;
        return 0;
    }

    ~ timers_poll()
    {

    }
private:
    int epfd;
    int active;
    std::map<int, timer *> timers_map;
    /* data */
} ;


#endif	/* TIMER_POLL_H */

