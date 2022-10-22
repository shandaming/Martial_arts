#ifndef GUI_EVENT_HANDLER_H
#define GUI_EVENT_HANDLER_H

#include <cstdint>
#include <iosfwd>
#include <vector>

class window;
class dispatcher;

class manager
{
public:
	manager();
	~manager();
};

// clang-format off

/**
 * Event category masks.
 *
 * These begin at 2^8 to allow for 8 bits for the event identifiers themselves.
 * This means ui_event can have up to 256 unique members. Since each mask needs
 * its own place value, we can have 24 categories since ui_event's underlying
 * type is 32 bits:
 *
 *                   USABLE CATEGORY BITS    NULL
 *                |------------------------|--------|
 *     MASK        000000000000000000000000 00000000
 *
 *                   ENCODED CATEGORY        EVENT
 *                |------------------------|--------|
 *     UI_EVENT    000000000000000000000000 00000000
 */
enum class event_category : uint32_t {
	/**
	 * Callbacks without extra parameters.
 	 * @note Some mouse events like MOUSE_ENTER don't send the mouse coordinates
 	 * to the callback function so they are also in this category.
 	 */
	general           = 1u << 8,

	/**
	 * Callbacks with a coordinate as extra parameter.
	 */
	mouse             = 1u << 9,

	/**
	 * Callbacks with the keyboard values (these haven't been determined yet).
	 */
	keyboard          = 1u << 10,

	touch_motion      = 1u << 11,
	touch_gesture     = 1u << 12,

	/**
	 * Callbacks with a sender aka notification messages. Like general events
	 * it has no extra parameters, but this version is only sent to the target
	 * and does not use the pre and post queue.
 	 */
	notification      = 1u << 13,

	/**
	 * Callbacks with a sender aka notification messages.
	 * Unlike the notifications this message is send through the chain. The event
	 * is sent from a widget all the way up to the window, who is always the
	 * receiver of the message (unless somebody grabbed it before).
	 */
	message           = 1u << 14,

	raw_event         = 1u << 15,
	text_input        = 1u << 16,
};
// clang-format on

constexpr uint32_t encode_category(const uint32_t input, const event_category mask)
{
	return input | static_cast<uint32_t>(mask);
}

// clang-format off
enum ui_event : uint32_t {
	DRAW                           = encode_category(1 , event_category::general),
	CLOSE_WINDOW                   = encode_category(2 , event_category::general),
	MOUSE_ENTER                    = encode_category(3 , event_category::general),
	MOUSE_LEAVE                    = encode_category(4 , event_category::general),
	LEFT_BUTTON_DOWN               = encode_category(5 , event_category::general),
	LEFT_BUTTON_UP                 = encode_category(6 , event_category::general),
	LEFT_BUTTON_CLICK              = encode_category(7 , event_category::general),
	LEFT_BUTTON_DOUBLE_CLICK       = encode_category(8 , event_category::general),
	MIDDLE_BUTTON_DOWN             = encode_category(9 , event_category::general),
	MIDDLE_BUTTON_UP               = encode_category(10, event_category::general),
	MIDDLE_BUTTON_CLICK            = encode_category(11, event_category::general),
	MIDDLE_BUTTON_DOUBLE_CLICK     = encode_category(12, event_category::general),
	RIGHT_BUTTON_DOWN              = encode_category(13, event_category::general),
	RIGHT_BUTTON_UP                = encode_category(14, event_category::general),
	RIGHT_BUTTON_CLICK             = encode_category(15, event_category::general),
	RIGHT_BUTTON_DOUBLE_CLICK      = encode_category(16, event_category::general),

	SDL_VIDEO_RESIZE               = encode_category(17, event_category::mouse),
	SDL_MOUSE_MOTION               = encode_category(18, event_category::mouse),
	MOUSE_MOTION                   = encode_category(19, event_category::mouse),
	SDL_LEFT_BUTTON_DOWN           = encode_category(20, event_category::mouse),
	SDL_LEFT_BUTTON_UP             = encode_category(21, event_category::mouse),
	SDL_MIDDLE_BUTTON_DOWN         = encode_category(22, event_category::mouse),
	SDL_MIDDLE_BUTTON_UP           = encode_category(23, event_category::mouse),
	SDL_RIGHT_BUTTON_DOWN          = encode_category(24, event_category::mouse),
	SDL_RIGHT_BUTTON_UP            = encode_category(25, event_category::mouse),
	SDL_WHEEL_LEFT                 = encode_category(26, event_category::mouse),
	SDL_WHEEL_RIGHT                = encode_category(27, event_category::mouse),
	SDL_WHEEL_UP                   = encode_category(28, event_category::mouse),
	SDL_WHEEL_DOWN                 = encode_category(29, event_category::mouse),
	SHOW_TOOLTIP                   = encode_category(30, event_category::mouse),
	SHOW_HELPTIP                   = encode_category(31, event_category::mouse),
	SDL_TOUCH_UP                   = encode_category(32, event_category::mouse),
	SDL_TOUCH_DOWN                 = encode_category(33, event_category::mouse),

	SDL_KEY_DOWN                   = encode_category(34, event_category::keyboard),

	SDL_TEXT_INPUT                 = encode_category(35, event_category::text_input), /**< An SDL text input (commit) event. */
	SDL_TEXT_EDITING               = encode_category(36, event_category::text_input), /**< An SDL text editing (IME) event. */

	SDL_ACTIVATE                   = encode_category(37, event_category::notification),
	NOTIFY_REMOVAL                 = encode_category(38, event_category::notification),
	NOTIFY_MODIFIED                = encode_category(39, event_category::notification),
	NOTIFY_REMOVE_TOOLTIP          = encode_category(40, event_category::notification),
	RECEIVE_KEYBOARD_FOCUS         = encode_category(41, event_category::notification),
	LOSE_KEYBOARD_FOCUS            = encode_category(42, event_category::notification),

	REQUEST_PLACEMENT              = encode_category(43, event_category::message),
	MESSAGE_SHOW_TOOLTIP           = encode_category(44, event_category::message),
	MESSAGE_SHOW_HELPTIP           = encode_category(45, event_category::message),

	SDL_TOUCH_MOTION               = encode_category(46, event_category::touch_motion),
	SDL_TOUCH_MULTI_GESTURE        = encode_category(47, event_category::touch_gesture),

	SDL_RAW_EVENT                  = encode_category(48, event_category::raw_event)
};
// clang-format on


constexpr bool is_in_category(const ui_event event, const event_category mask)
{
	const uint32_t asu32 = static_cast<uint32_t>(mask);
	return (event & asu32) == asu32;
}

constexpr event_category get_event_category(const ui_event event)
{
	// Zero-out the first 8 bits since those encode the ui_event value, which we don't want.
	return static_cast<event_category>((event >> 8u) << 8u);
}

void connect_dispatcher(dispatcher* dispatcher);
void disconnect_dispatcher(dispatcher* dispatcher);

std::vector<dispatcher*>& get_all_dispatchers();

void init_mouse_location();
void capture_mouse(dispatcher* dispatcher);
void release_mouse(dispatcher* dispatcher);

void capture_keyboard(dispatcher* dispatcher);

std::ostream& operator<<(std::ostream& stream, const ui_event event);

extern std::vector<window*> open_window_stack;

void remove_from_window_stack(window* window);

bool is_in_dialog();

#endif
