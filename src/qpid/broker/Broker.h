#ifndef _Broker_
#define _Broker_

/*
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 *
 */

#include "ConnectionFactory.h"
#include "ConnectionToken.h"
#include "DirectExchange.h"
#include "DtxManager.h"
#include "ExchangeRegistry.h"
#include "MessageStore.h"
#include "QueueRegistry.h"
#include "qpid/Options.h"
#include "qpid/Plugin.h"
#include "qpid/Url.h"
#include "qpid/framing/FrameHandler.h"
#include "qpid/framing/OutputHandler.h"
#include "qpid/framing/ProtocolInitiation.h"
#include "qpid/sys/Acceptor.h"
#include "qpid/sys/Runnable.h"

#include <vector>

namespace qpid { 

namespace framing {
class HandlerUpdater;
}

namespace broker {

/**
 * A broker instance. 
 */
class Broker : public sys::Runnable, public Plugin::Target
{
  public:
    struct Options : public qpid::Options {
        Options(const std::string& name="Broker Options");
        
        uint16_t port;
        int workerThreads;
        int maxConnections;
        int connectionBacklog;
        std::string store;      
        long stagingThreshold;
    };
    
    virtual ~Broker();

    Broker(const Options& configuration);
    static shared_ptr<Broker> create(const Options& configuration);
    static shared_ptr<Broker> create(int16_t port = TcpAddress::DEFAULT_PORT);

    /**
     * Return listening port. If called before bind this is
     * the configured port. If called after it is the actual
     * port, which will be different if the configured port is
     * 0.
     */
    virtual int16_t getPort() const;

    /** Return the broker's URL. */
    virtual std::string getUrl() const;
    
    /**
     * Run the broker. Implements Runnable::run() so the broker
     * can be run in a separate thread.
     */
    virtual void run();

    /** Shut down the broker */
    virtual void shutdown();

    /** Register a handler updater. */
    void add(const shared_ptr<framing::HandlerUpdater>&);
    
    /** Apply all handler updaters to a handler chain pair. */
    void update(framing::ChannelId, framing::FrameHandler::Chains&); 
    
    MessageStore& getStore() { return *store; }
    QueueRegistry& getQueues() { return queues; }
    ExchangeRegistry& getExchanges() { return exchanges; }
    uint64_t getStagingThreshold() { return stagingThreshold; }
    DtxManager& getDtxManager() { return dtxManager; }

  private:
    sys::Acceptor& getAcceptor() const;

    Options config;
    sys::Acceptor::shared_ptr acceptor;
    const std::auto_ptr<MessageStore> store;
    typedef std::vector<shared_ptr<framing::HandlerUpdater> > HandlerUpdaters;

    QueueRegistry queues;
    ExchangeRegistry exchanges;
    uint64_t stagingThreshold;
    ConnectionFactory factory;
    DtxManager dtxManager;
    HandlerUpdaters handlerUpdaters;

    static MessageStore* createStore(const Options& config);
};

}}
            


#endif  /*!_Broker_*/
