#include "CWindow.hpp"

void CWindow::Initialize()
{
  pConnect_ = ::xcb_connect(nullptr, nullptr);
  assert(::xcb_connection_has_error(pConnect_));

  pScreen_ = ::xcb_setup_roots_iterator(::xcb_get_setup(pConnect_)).data;

  vdl::uint32_t ValueList[] = { pScreen_->black_pixel, XCB_EVENT_MASK_KEY_RELEASE | XCB_EVENT_MASK_EXPOSURE };

  Window_ = ::xcb_generate_id(pConnect_);
  ::xcb_create_window(pConnect_, 
    XCB_COPY_FROM_PARENT,
    Window_,
    pScreen_->root, 
    0,
    0,
    WindowSize_.x, 
    WindowSize_.y,
    0,
    XCB_WINDOW_CLASS_INPUT_OUTPUT, 
    pScreen_->root_visual,
    XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK,
    ValueList);

  xcb_intern_atom_cookie_t cookie = ::xcb_intern_atom(pConnect_, 1, 12, "WM_PROTOCOLS");
  xcb_intern_atom_reply_t *reply = ::xcb_intern_atom_reply(pConnect_, cookie, 0);

  xcb_intern_atom_cookie_t cookie2 = ::xcb_intern_atom(pConnect_, 0, 16, "WM_DELETE_WINDOW");
  pDestoryReply_ = ::xcb_intern_atom_reply(pConnect_, cookie2, 0);

  ::xcb_change_property(pConnect_, XCB_PROP_MODE_REPLACE, Window_, pDestoryReply_->atom, 4, 32, 1,
    &pDestoryReply_->atom);
  ::free(reply);

  ::xcb_map_window(pConnect_, Window_);
}

void CWindow::Show(bool _isShow)
{

}
