/*
 *
 * Copyright (c) 2006 The Apache Software Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#ifndef _LFAcceptor_
#define _LFAcceptor_

#include <vector>
#include "apr-1/apr_network_io.h"
#include "apr-1/apr_poll.h"
#include "apr-1/apr_time.h"

#include "qpid/io/Acceptor.h"
#include "qpid/concurrent/APRMonitor.h"
#include "qpid/concurrent/APRThreadFactory.h"
#include "qpid/concurrent/APRThreadPool.h"
#include "qpid/io/LFProcessor.h"
#include "qpid/io/LFSessionContext.h"
#include "qpid/concurrent/Runnable.h"
#include "qpid/io/SessionContext.h"
#include "qpid/io/SessionHandlerFactory.h"
#include "qpid/concurrent/Thread.h"

namespace qpid {
namespace io {

    class LFAcceptor : public virtual Acceptor
    {
        class APRPool{
        public:
            apr_pool_t* pool;
            APRPool();
            ~APRPool();
        };

        APRPool aprPool;
        LFProcessor processor;
        apr_socket_t* socket;
        const int max_connections_per_processor;
        const bool debug;
        const int connectionBacklog;

        volatile bool running;

    public:
	LFAcceptor(bool debug = false, 
                   int connectionBacklog = 10, 
                   int worker_threads = 5, 
                   int max_connections_per_processor = 500);
        virtual int16_t bind(int16_t port);
        virtual int16_t getPort() const;
        virtual void run(SessionHandlerFactory* factory);
        virtual void shutdown();
	virtual ~LFAcceptor();
    };

}
}


#endif
