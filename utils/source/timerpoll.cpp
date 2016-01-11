/* 
 * File:   timer_poll.cpp
 * Author: Administrator
 *
 * Created on 2013年7月4日, 下午3:17
 */

#include <cstdlib>
#include "timerpoll.h"
#include <iostream>

using namespace std;

timer::timer(const timer& ptimer)
{
    timer_internal = ptimer.timer_internal;
    cb = ptimer.cb;
    timer_id = ptimer.timer_id;
    repeat = ptimer.repeat;
    userdata = ptimer.userdata;
}

timer & timer::operator =(const timer& ptimer)
{
    if (this == &ptimer)
    {
        return *this;
    }

    timer_internal = ptimer.timer_internal;
    cb = ptimer.cb;
    timer_id = ptimer.timer_id;
    repeat = ptimer.repeat;
    userdata = ptimer.userdata;
    return *this;
}

int timer::timer_start()
{
    struct itimerspec ptime_internal = {{0}};
    ptime_internal.it_value.tv_sec = (int) timer_internal;
    ptime_internal.it_value.tv_nsec = (timer_internal - (int) timer_internal)*1000000;
    if(repeat)
    {
        ptime_internal.it_interval.tv_sec = ptime_internal.it_value.tv_sec;
        ptime_internal.it_interval.tv_nsec = ptime_internal.it_value.tv_nsec;
    }
    
    timerfd_settime(timer_id, 0, &ptime_internal, NULL);
    return 0;
}

int timer::timer_stop()
{
    close(timer_id);
    return 0;
}

int timer::timer_modify_internal(double timer_internal)
{
    this->timer_internal = timer_internal;
    timer_start();

    return 0;
}

int timers_poll::timers_poll_add_timer(timer * ptimer)
{
    int timer_id = ptimer->timer_get_id();
    struct epoll_event ev;
    ev.data.fd = timer_id;
    ev.events = EPOLLIN | EPOLLET;
    timers_map.insert(make_pair<int, timer *>(timer_id, ptimer)); //add or modify
    epoll_ctl (epfd, EPOLL_CTL_ADD, timer_id, &ev);
    ptimer->timer_start();

    return 0;
}

int timers_poll::timers_poll_del_timer(timer* ptimer)
{
    int timer_id = ptimer->timer_get_id();
    struct epoll_event ev;
    ev.data.fd = timer_id;
    ev.events = EPOLLIN | EPOLLET;
    epoll_ctl (epfd, EPOLL_CTL_DEL, timer_id, &ev);
    timers_map.erase(timer_id);
    delete ptimer;
    
    return 0;
}

int timers_poll::run()
{
    char buf[128] ={0};
    for (; active ; )
    {
        struct epoll_event events[MAXFDS] ={{0}};
        int nfds = epoll_wait (epfd, events, MAXFDS, -1);
        for (int i = 0; i < nfds; ++i)
        {
            std::map<int, timer *>::iterator itmp = timers_map.find(events[i].data.fd);
            if (itmp != timers_map.end())
            {
                //timer ptimer = itmp->second;
                while (read(events[i].data.fd, buf, 128) > 0);
                itmp->second->get_user_callback()(itmp->second);
            }
        }
    }

    return 0;
}


