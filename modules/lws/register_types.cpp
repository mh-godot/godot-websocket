/*************************************************************************/
/*  register_types.cpp                                                   */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2017 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2017 Godot Engine contributors (cf. AUTHORS.md)    */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/
#include "register_types.h"
#include "error_macros.h"
#ifdef JAVASCRIPT_ENABLED
#include "esws_peer.h"
#include "esws_client.h"
#include "esws_server.h"
#include "emscripten.h"
#else
#include "lws_peer.h"
#include "lws_client.h"
#include "lws_server.h"
#endif

void register_lws_types() {
#ifdef JAVASCRIPT_ENABLED
	EM_ASM({
		var IDHandler = {};
		IDHandler["ids"] = [];
		// get an integer ID for a JS object. this keeps a reference to it, preventing GC'ing
		IDHandler["add"] = function(obj) {
			var id = IDHandler.ids.length;
			IDHandler.ids[id] = obj;
			return id;
		};
		// get a JS object from an integer ID
		IDHandler["get"] = function(id) {
			return IDHandler.ids[id];
		};
		// releases an object that has an ID. this allows it to be GD'd
		IDHandler["remove"] = function(id) {
			IDHandler.ids[id] = null;
		};
		Module["IDHandler"] = IDHandler;
	});
	ESWSPeer::make_default();
	ESWSClient::make_default();
	ESWSServer::make_default();
#else
	LWSPeer::make_default();
	LWSClient::make_default();
	LWSServer::make_default();
#endif

	ClassDB::register_custom_instance_class<WebSocketServer>();
	ClassDB::register_custom_instance_class<WebSocketClient>();
	ClassDB::register_custom_instance_class<WebSocketPeer>();
}

void unregister_lws_types() {}
