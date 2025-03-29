#pragma once



void inspector_draw_attributes(BaseObject* selected_p) {
	for (Property property : selected_p->properties) {
		if (property == Property::TRANSFORM) {
			

		}

		if (property == Property::LIGHT) {
			PointLight* obj = static_cast<PointLight*>(selected_p);
			ImGui::SeparatorText("Light");
			ImGui::SliderFloat("Radius", &(obj->radius), 0.0, 50.0);
			ImGui::SliderFloat("Strength", &(obj->strength), 0.0, 50.0);
			ImGui::Separator();

			float color[3];

			color[0] = obj->color.x;
			color[1] = obj->color.y;
			color[2] = obj->color.z;

			ImGui::ColorPicker3("Light Color", color);


			obj->color = glm::vec3(color[0], color[1], color[2]);
		}
	}
}