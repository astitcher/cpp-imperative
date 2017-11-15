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

#include "options.hpp"

#include "proton/imperative.hpp"

#include <iostream>
#include <string>
#include <vector>

using proton::container;
using proton::message;
using proton::receiver;
using proton::sender;
using proton::receiver_options;
using proton::source_options;

using std::cout;
using std::string;
using std::vector;

void send_request(sender& sender, const string& msg, const string& reply_address) {
    message req;
    req.body(msg);
    req.reply_to(reply_address);
    sender.send(req);
}

void process(proton::container &c, const string& url, vector<string>& requests) {
    sender sender = c.open_sender(url);

    // Create a receiver requesting a dynamically created queue for the message source.
    receiver receiver = sender.connection().open_receiver("", source(dynamic(true)));

    send_request(sender, requests[0], receiver.source().address());

    for (message& response : receiver.messages()) {
        
        cout << requests.front() << " => " << response.body() << std::endl;
        requests.erase(requests.begin());

        if (requests.empty()) return;

        send_request(sender, requests[0], receiver.source().address());
    }
}

int main(int argc, char **argv) {
    std::string url("127.0.0.1:5672/examples");
    example::options opts(argc, argv);

    opts.add_value(url, 'a', "address", "connect and send to URL", "URL");

    try {
        opts.parse();

        vector<string> requests = {
            "Twas brillig, and the slithy toves",
            "Did gire and gymble in the wabe.",
            "All mimsy were the borogroves,",
            "And the mome raths outgrabe."
        };

        container c;
        
        process(c, url, requests);

        return 0;
    } catch (const example::bad_option& e) {
        std::cout << opts << std::endl << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 1;
}
