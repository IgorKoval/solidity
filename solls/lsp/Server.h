#pragma once

#include <lsp/protocol.h>
#include <lsp/InputHandler.h>
#include <lsp/OutputGenerator.h>
#include <lsp/Logger.h>

#include <json/value.h>

#include <functional>
#include <optional>
#include <ostream>
#include <unordered_map>
#include <variant>

namespace lsp {

class Transport;

/// Solidity Language Server, managing one LSP client.
class Server: public Logger
{
private:
	Server(Server const&) = delete;
	Server& operator=(Server const&) = delete;

public:
	using Id = protocol::Id;

	/// Constructs a Language Server that is communicating over stdio via JSON-RPC.
	///
	/// @param _client the transport layer to the connected client
	/// @param _logger a logging stream, used internally for logging debug/warning/error messages.
	explicit Server(Transport& _client);

	virtual ~Server() = default;

	/// Loops over incoming messages via the transport layer until shutdown condition is meat.
	///
	/// The standard shutdown condition is when the maximum number of consecutive failures
	/// has been exceeded.
	///
	/// @return an error code suitable to return in standard main().
	int run();

	/// Handles a raw client message
	void handleMessage(std::string const& _message);

	// Client-to-Server messages
	virtual void operator()(protocol::CancelRequest const&) = 0;
	virtual void operator()(protocol::InitializeRequest const&) = 0;
	virtual void operator()(protocol::InitializedNotification const&) {};
	virtual void operator()(protocol::DidOpenTextDocumentParams const&) {}
	virtual void operator()(protocol::DidChangeTextDocumentParams const&) {}
	virtual void operator()(protocol::DidCloseTextDocumentParams const&) {}

	/// Sends a message to the client.
	///
	/// @param _id an optional request ID that this response relates to
	/// @param _message the message to send to the client
	void reply(lsp::protocol::Id const& _id, lsp::protocol::Response const& _message);
	void error(lsp::protocol::Id const& _id, std::string const& _message);
	void notify(lsp::protocol::Notification const& _message);

	void log(protocol::MessageType _type, std::string const& _message) override;

protected:
	[[deprecated]] void sendReply(Json::Value const& _response, std::optional<Id> _requestId = std::nullopt);

	Transport& client() noexcept { return m_client; }

private:
	Transport& m_client;
	InputHandler m_inputHandler;
	OutputGenerator m_outputGenerator;
};

} // namespace solidity

