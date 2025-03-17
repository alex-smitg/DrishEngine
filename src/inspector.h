#pragma once



void inspector_draw_attributes(BaseObject* selected_p) {
	for (Property property : selected_p->properties) {
		if (property == Property::TRANSFORM) {
			ImGui::SeparatorText("Position");
			ImGui::PushItemWidth(64.0f);
			ImGui::Text("X");
			ImGui::SameLine();
			ImGui::DragFloat("##x", &(selected_p->transform.position.x), 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::SameLine();
			ImGui::Text("Y");
			ImGui::SameLine();
			ImGui::DragFloat("##y", &(selected_p->transform.position.y), 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::SameLine();
			ImGui::Text("Z");
			ImGui::SameLine();
			ImGui::DragFloat("##z", &(selected_p->transform.position.z), 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::PopItemWidth();

			ImGui::SeparatorText("Rotation");
			ImGui::PushItemWidth(64.0f);
			ImGui::Text("X");
			ImGui::SameLine();
			ImGui::DragFloat("##rx", &(selected_p->transform.rotation.x), 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::SameLine();
			ImGui::Text("Y");
			ImGui::SameLine();
			ImGui::DragFloat("##ry", &(selected_p->transform.rotation.y), 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::SameLine();
			ImGui::Text("Z");
			ImGui::SameLine();
			ImGui::DragFloat("##rz", &(selected_p->transform.rotation.z), 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::PopItemWidth();

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