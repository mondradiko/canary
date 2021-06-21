# Overview

This is the design document for the Mondradiko UI framework (still unnamed), a
spatial UI framework designed for easy integration, high customizability, and
use in an XR environment.

The target audience of this document consists of developers who are interested
in embedding this framework, developers who are interested in contributing to
this framework, UI script authors who would like to understand the internals and
design philosophies of to write better scripts, as well as any non-developers
who would like to learn about and influence open-source XR UI development at
its source.

# Goals

## Customizability

The dream of this UI framework is to let users customize their menus, HUDs,
color scheme, widget themes, etc. however they want, and then be able to bring
that UI configuration across separate servers in a metaverse platform, or even
across metaverse platforms altogether.

If users will be spending hours at a time in online metaverse platforms, having
this level of customizability would provide some much-needed comfort and
personal expression.

## Flexibility

Another goal of the UI framework is flexibility. This can mean a couple of
different things.

First of all, it means that the UI framework is able to adapt to new XR
technologies that change how XR UIs are interacted with. If this framework
is meant to be the go-to choice for XR-friendly UI development, then it's
essential that it can grow with XR hardware as it matures.

Second of all, it means that the UI framework must be simple, and easy to embed
into XR platforms that need it. By abstracting its logic away from any
higher-level game logic, platform requirements, or specific hardware mappings,
its design can be more easily integrated with platforms that already have their
own solutions to those design facets.

## Performance

The faster a UI library runs, the more functionality user scripts can pack in,
and the more room they have to play with new ideas within a given frame budget.
This extra room may accomodate higher animation fidelity, poly count, or complex
interaction mechanics. The UI framework also needs to stay lightweight, and out
of the way of its host environments, for integration's sake. This will mea
minimizing the overhead of the translation layers between user scripts and the
host's API.

# UI Panels

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

## Attributes

More panel attributes will be added over time as the needs of a panel change.
For now, panels are represented as simple, colored rectangles in 3D space.

Current attributes:
- color (RGBA)
- position (XYZ)
- orientation (quaternion)
- size (width, height)

Eventually, panels will also be curved. Curved panels are much easier to
interact with in terms of hand input, as the curve can be made to fit user's
wrists or other arm joints, providing a more comfortable experience. They can
also look very visually appealing. However, they add significantly more
complexity to the renderer, and make [embedding](#processing-triangle-lists)
harder to implement. For this reason, curved panels will almost certainly be
made a "version 2.0" feature that is not high priority.

Future attributes:
- horizontal/vertical curve (expressed as radians)
- corner rounding (one radius per corner)

# Spaces

The user interface framework operates in four different spaces: panel space,
world space, stage space, and view space.

## Panel Space

Panel space is in a two-dimensional (2D) plane local to each panel. This is the
space that [rendering commands](#rendering) is recorded in, and the space that
[input events](#input) are transposed to.

The X axis goes from left to right, and the Y axis goes from top to bottom.
The origin (0, 0) is in the center of the panel.

```
   |
   |
---o--->  X
   |
   |
   V

   Y
```

## World Space

World space is three-dimensional "global" space, where "global" space is
host-defined. For example, a map, video stream, or announcements board in a game
world would be located within world space.

## Stage Space

Stage space is a user-subjective 3D space that maps 1:1 to the user's real-life
playing space. Panels that need to stay "bound" to a certain user would be in
this space. Examples include game menus, inventories, option menus, browsers,
and so on.

> TODO(marceline-cramer): are stage spaces and view spaces necessary, or can
> they be eliminated from this framework's scope for simplicity's sake? if they
> are necessary, are they high priority, or should they be developed in v2.0?

## View Space

View space is relative to a viewer's headset. Panels placed in view space are
*non-interactable.* Notification queues, game HUDs, and status bars would all be
in view space.

## Transposition

# Scripting

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

"UI scripts," then, are WebAssembly modules that implement all UI functionality
within the framework, including widget drawing, event handling, and animations.
This design lends itself to [portability](#flexibility), as well as to
virtually limitless [customizability](#customizability), while not sacrificing
[performance](#performance).

The ultimate purpose of the UI scripting environment is to provide UI script
authors with a playground to make whatever UI engine they want, while still
grounding it in usefulness and consistency by
[interfacing with it properly](#protocol).

# Protocol

## FlatBuffers

## Language Support

# Rendering

## Why Triangles?

## Immediate Mode

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

## Draw Commands

UI contents are composed out of a simple triangle list, per panel. The UI
script can call a host function to draw single triangles, helper functions to
draw common primitives (circles, rounded rectangles, polygons, etc.), or a host
function that directly draws vertex and index buffers from within Wasm memory.

The last option, drawing buffers directly, allows the UI script to cache its
draw calls or quickly render complex primitives without spending a host
function call for each triangle every frame.

## Glyphs

> TODO(marceline-cramer): make discussion issue

# Audio

> TODO(marceline-cramer): make discussion issue

# Embedding

The UI framework can be embedded into larger XR applications ("host
environments") to provide [flexible](#flexibility),
[customizable](#customizability), and [lightweight](#performant) UI
functionality.

Natively, this would be done by linking to the UI framework as a library. In
the browser, the UI framework would be integrated as a WebAssembly module. 

## Processing Triangle Lists

To render any UI content, the host environment must consume the triangle lists
generated by each script's [draw commands](#draw-commands), and draw them to the
display. This is done per-panel, and the host environment is responsible for
converting the [panel attributes](#attributes) into transform matrices, uniform
buffers, shader code, etc. that can draw each panel's triangle list.

Curved panels make this significantly more complex, as the triangle lists would
have to be dynamically modified by the host environment and mapped to a curved
surface in 3D space. See [panel attributes](#attributes).

## Adding Input Methods

### Mouse Input

### Pointer Input

### Hand Input

## Sending/Receiving Widget Data 

## Hosting over IPC

## Hosting to Remote Clients

# Input

## Event Types

## Panel Geometry

## Keyboard

# Setup

## Panel Binding

Once a panel is created, it must be "bound" to a script. This done by first
registering a panel with a script wrapper object, giving it an integer key,
then passing that key, as a Wasm `i32` type, to a function exported by the Wasm
script. This function returns another `i32` key to be used by the framework as a
script-defined identifier for that panel's binding. This second `i32` key is
used by the framework to identify the panel binding to the script.

Both `i32`s are basically opaque identifiers. The first one, passed to the
script environment by the framework, is the framework-internal integer
identifier for that panel. This can be used by the script to call
[draw commands](#draw-commands) as well as to read and write
[panel attributes](#attributes).

The second opaque identifier is script defined, and could refer to an index into
a static array containing data structures for each panel implementation, or more
realistically, a literal pointer into Wasm memory to that structure. For
example, in an AssemblyScript Wasm script, the binding function can instantiate
a new object of a class implementing their panel functionality, then return that
object handle as an `i32`. This `i32` is used by the framework as the first
argument to script callbacks, so in AssemblyScript, callbacks could be written
as methods in that class.

AssemblyScript example:

```ts
class PanelImpl {
	constructor(public panel: UiPanel) {
		// read panel attributes
		// initialize data members
		// etc.
	}

	update(dt: f64): void {
		// animate widgets
		// submit draw commands
	}
}

export function bind_panel(panel: UiPanel): PanelImpl {
	return new PanelImpl(panel);
}

export function update(self: PanelImpl, dt: f64): void {
	self.update(dt);
}
```

> TODO(marceline-cramer): can exported callbacks be set by the constructor
> and bound for unique panel instances? how does that work with Wasm function
> tables? how well does AssemblyScript handle those function tables? is it
> worth the added complexity? if so, is implementing this high priority?

## Sources/Sinks

## Widget Construction

