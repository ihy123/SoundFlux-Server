#include "pch.h"
#include "SoundFlux.h"

using namespace nana;

int main(int argc, char* argv[]) {
	SoundFlux soundFlux;
	soundFlux.show();
	exec();
	return 0;
}

SoundFlux::SoundFlux()
	: form(API::make_center(600, 600), appearance(true, true, true, false, true, false, false)) {
	caption((const char*)u8"Здравствуй world!");

	bt.create(*this);
	bt.caption("Button");
	bt.events().click([&](const nana::arg_click& arg) {
		if (arg.mouse_args->button == mouse::left_button) {
			printf("Button clicked!\n");
		}
	});

	layout.bind(*this);
	layout.div("vert <><weight=30% <><weight=30% button><>><>");
	layout["button"] << bt;
	layout.collocate();
}
