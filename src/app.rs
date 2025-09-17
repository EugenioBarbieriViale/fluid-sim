extern crate winit;

use std::sync::Arc;
use crate::wgpu_ctx::WgpuCtx;

use winit::application::ApplicationHandler;
use winit::event::WindowEvent;
use winit::event_loop::{ActiveEventLoop};
use winit::window::{Window, WindowId};

#[derive(Default)]
pub struct App<'window> {
    window: Option<Arc<Window>>,
    wgpu_ctx: Option<WgpuCtx<'window>>,
}

impl<'window> ApplicationHandler for App<'window> {
    fn resumed(&mut self, event_loop: &ActiveEventLoop) {
        if self.window.is_none() {
            let window_attrs = Window::default_attributes();
            let window = Arc::new(event_loop.create_window(window_attrs).unwrap());
            self.window = Some(window.clone());

            self.wgpu_ctx = Some(WgpuCtx::new(window.clone()));
        }
    }

    fn window_event(&mut self, event_loop: &ActiveEventLoop, id: WindowId, event: WindowEvent) {
        match event {
            WindowEvent::CloseRequested => event_loop.exit(),
            WindowEvent::RedrawRequested => {
                if let Some(wgpu_ctx) = self.wgpu_ctx.as_mut() {
                    wgpu_ctx.draw();
                }
            },
            _ => {},
        }
    }
}
