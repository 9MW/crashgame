#pragma once

namespace  pj2 {
	namespace n_seri {
		namespace KStr {
			static const std::string RenderSetting = "RenderSetting";
		}
			static const std::string animstr[] = {
				"dieing ",
				"move",
				"attack",
				"attackmove",
				"idle",
				"pick",
				"put",
				"count",
				"reload"
			};
		namespace gunpropname {
			const std::string
				gun_name = "gun_name",
				shotAudio = "shotAudio",
				reloadaud = "reloadaud",
				finishloadaud = "finishloadaud",
				description = "descption",
				reload_time = "reload_time",
				bulleti = "bullet id",
				capacity = "capacity",
				firevel = "attackvel";
		}
		namespace compname {
			static const std::string propstrmap[] = {
					"speed",
					"feq",
					"energy",
					"hp",
					"visiableRange",
					"attackrangesq",
					"damage",
					"Armor"
			};
			static const std::string
				trans = "transform",
				CST = "Command",
				statetick="tick",
				load="load",
				bp = "basic props";
			
		}
	}
}