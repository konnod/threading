cmake_minimum_required(VERSION 3.12)

project(threading VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 14)

link_libraries(pthread)

set(SOURCE_FILES
        src/ThreadBase.h
        src/ThreadPoolBase.h
        src/ThreadSafeBase.h
        src/task_thread/TaskThread.h
        src/task_thread/Task.h
        src/task_thread/ITask.h
        src/query_thread/QueryBase.h
        src/query_thread/QueryThreadPool.h
        src/query_thread/QueryThreadSimple.h
        src/query_thread/QueryThreadBase.h
        src/query_thread/QueryQueueBase.h
        src/query_thread/QueryThreadTimeout.h
        src/query_thread/QueryThreadPoolThread.h
        src/utils/PredicateCondition.h
        src/utils/GuardedMap.h
        src/utils/GuardedDeque.h
        src/utils/Condition.h
        src/utils/SPtrFactoryBase.h
        src/utils/PtrDeclBase.h examples/task.cpp)

add_executable(threading ${SOURCE_FILES})

include_directories(src/)
