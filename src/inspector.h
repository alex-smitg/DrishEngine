#pragma once



void inspector_draw_attributes(BaseObject* selected_p) {
	for (Attribute attribute : selected_p->attributes) {
		if (attribute == Attribute::POSITION) {
			ImGui::SeparatorText("Position");
			ImGui::PushItemWidth(64.0f);
			ImGui::Text("X");
			ImGui::SameLine();
			ImGui::DragFloat("##x", &(selected_p->transform.position.x), 1.0f, 0.0f, 0.0f, "%.2f");
			ImGui::SameLine();
			ImGui::Text("Y");
			ImGui::SameLine();
			ImGui::DragFloat("##y", &(selected_p->transform.position.y), 1.0f, 0.0f, 0.0f, "%.2f");
			ImGui::SameLine();
			ImGui::Text("Z");
			ImGui::SameLine();
			ImGui::DragFloat("##z", &(selected_p->transform.position.z), 1.0f, 0.0f, 0.0f, "%.2f");
			ImGui::PopItemWidth();

		}

		if (attribute == Attribute::LIGHT) {
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