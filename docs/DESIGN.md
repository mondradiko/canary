# Introduction

This is the design document for the Mondradiko UI framework (still unnamed), a
spatial UI framework designed for easy integration, high customizability, and
use in an XR environment.

## Goals

### Flexibility

### Customizability

### Performance

# Design Overview

## UI Panels

The centerpiece of the UI framework is the "panel," a floating, rectangular
object that users primarily interact with. Widgets are drawn onto the
panels, and users interact with those widgets with controllers, hand trackers,
etc., to trigger events to be processed by the host environment.

Panels can be positioned and oriented arbitrarily in 3D space, and can be any
width and height, or even curved. They can also be organized into different
arrangements to provide different interface layouts.

Widgets cannot be drawn outside of panels. This allows the host environment to
restrict panel placement and size, so that it's safer to host user UIs that
may potentially be malicious. A malicious UI might place panels in front of
user's faces, draw large or distracting shapes, or otherwise visually
interfere with user experience. This is why panels are a necessary construct;
to sandbox UI behavior.

### Attributes

- color (RGBA)
- position (XYZ)
- orientation (quaternion)
- size (width, height)
- horizontal/vertical curve (expressed as radians)
- corner rounding (one radius per corner)

## UI Scripts

The core functionality of the UI framework comes from its scripting,
which is powered by [WebAssembly](https://webassembly.org/). WebAssembly is a
perfect choice for this scripting environment because:

- It's fast. Browsers compile it to native machine code at runtime, so it far
  outpaces higher-level programming languages like TypeScript and JavaScript.
- It's safe. WebAssembly operates on linear memory, but the access to this
  memory is 100% sandboxed, and Wasm cannot access data that it is not
	explicitly given.
- It's free-form. Although the raw Wasm bytecode format is more similar to
  typical assembly language than anything the average UI designer will want
	to work with, many languages can compile to Wasm, including
	[TypeScript (AssemblyScript)](https://www.assemblyscript.org/),
	[Rust](https://www.rust-lang.org/what/wasm),
	[C/C++](https://emscripten.org/),
	and [many, many more](https://github.com/appcypher/awesome-wasm-langs).
	UI authors have plenty of options for how to write their UI implementations.
- It's portable. WebAssembly can run both in the browser and natively, helping
  bridge the gap between native and browser-based XR platforms. Wasm modules
	are small in size, and can be easily distributed and deployed, meaning that
	for the user, bringing their personal, customized UI between apps is simple.

## Rendering

### Immediate Mode

All UI rendering is done in immediate mode. If you are familiar with user
interface framework design, this may seem unusual to you, considering that
[a major design goal of this framework is performance](#performance). However,
as it turns out, immediate-mode rendering can actually be much more performant
than DOM-based rendering, as the framework does not have to keep track of all
of the modified state in its DOM. The complexity of keeping a DOM up-to-date is
further increased by user scripts.

By delegating responsibility of managing the render state of each panel to the
script implementations, load complexity is moved off of the core framework,
making it easier to embed, extend, and develop. Meanwhile, if it turns out that
a script implementation's representation of the UI is similar to a DOM, then it
may benefit from implementing some DOM-like rendering logic itself, giving the
UI framework as a whole the best of both worlds.

### Draw Commands

## Protocol

### FlatBuffers

### Language Support

## Embedding

### Requirements

### Adding Input Methods

### Hosting over IPC

### Hosting to Remote Clients

# Input

## Event Types

## Panel Geometry

## Keyboard

# Setup

## Panel Binding

## Sources/Sinks

## Widget Construction

# Widget Types

## Button

## ToggleButton

## Slider

## Radio

