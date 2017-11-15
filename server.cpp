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

#include "proton/imperative.hpp"

#include <iostream>
#include <map>
#include <string>
#include <cctype>

using proton::connection;
using proton::container;
using proton::message;
using proton::receiver;
using proton::sender;

using std::cout;
using std::map;
using std::string;

string to_upper(string uc) {
    for (auto& c : uc) {
        c = static_cast<char>(std::toupper(c));
    }
    return uc;
}

void process(container c, const string& url, const string& addr) {
    connection conn = c.connect(url);
    receiver receiver = conn.open_receiver(addr);
    map<string, sender> senders;
    
    cout << "Server connected to " << url << std::endl;

    for (message m : receiver.messages()) {
        cout << "Received " << m.body() << std::endl;

        string reply_to = m.reply_to();
        message reply;

        reply.to(reply_to);
        reply.body(to_upper(proton::get<std::string>(m.body())));
        reply.correlation_id(m.correlation_id());

        if (!senders[reply_to]) {
            senders[reply_to] = conn.open_sender(reply_to);
        }

        senders[reply_to].send(reply);
    }
}

int main(int argc, char** argv) {
    try {
        std::string conn_url = argc > 1 ? argv[1] : "//127.0.0.1:5672";
        std::string addr = argc > 2 ? argv[2] : "examples";

        proton::container c;
        process(c, conn_url, addr);

        return 0;
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 1;
}
