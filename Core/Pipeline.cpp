#include "Pipeline.h"

#include "Utils/Random.h"

#include "Object.h"

#include "Sample.h"

#include "DFT.h"

namespace Simulation {

	std::vector<Sample> Samples;

	const auto MaxSamples = 64000;

	float CurrentTime = glfwGetTime();
	float Frametime = 0.0f;
	float DeltaTime = 0.0f;

	glm::vec2 CurrentCursorPos;
	glm::vec2 PrevCursorPos;

	bool DrawingMode = true;
	bool ComputerDrawMode = false;
	long ComputerDrawObjects = 0;

	int DFTQ = 2048;

	// DFT :
	bool ShouldGenDFT = false;
	DFT2D ImageDFT;
	float DFTDeltaTime = -1.;

	std::vector<Object> Objects;

	class RayTracerApp : public Simulation::Application
	{
	public:

		bool vsync;

		RayTracerApp()
		{
			m_Width = 800;
			m_Height = 600;
		}

		void OnUserCreate(double ts) override
		{

		}

		void OnUserUpdate(double ts) override
		{
			glfwSwapInterval((int)vsync);

			GLFWwindow* window = GetWindow();

		}

		void OnImguiRender(double ts) override
		{
			ImGuiIO& io = ImGui::GetIO();
			if (ImGui::Begin("Debug/Edit Mode")) {

				ImGui::Text("Instructions : ");
				ImGui::Text("R -> Reset");
				ImGui::Text("LMB/Mouse -> Draw");
				ImGui::Text("D -> Toggle drawing mode");
				ImGui::Text("F -> Toggle computer drawing mode");
				ImGui::Text("G -> Generate 32x32 DFT and render it");
				ImGui::NewLine();
				ImGui::NewLine();
				ImGui::NewLine();

				if (ComputerDrawMode) {
					ImGui::Text("COMPUTER Drawing Mode is ACTIVE (toggle with F)");
				}
				else {
					ComputerDrawObjects = 0;
					ImGui::Text("COMPUTER Drawing Mode is IN-ACTIVE (toggle with F)");
				}

				if (DrawingMode)
					ImGui::Text("Drawing Mode is ACTIVE");
				else
					ImGui::Text("Drawing Mode is IN-ACTIVE");


				ImGui::Text("Samples : %d", Samples.size());

				if (DFTDeltaTime > 0) {
					ImGui::Text("DFT Delta Time : %lf", DFTDeltaTime);
				}
			
			} ImGui::End();
		}

		void OnEvent(Simulation::Event e) override
		{
			ImGuiIO& io = ImGui::GetIO();

			if (e.type == Simulation::EventTypes::MousePress && !ImGui::GetIO().WantCaptureMouse && GetCurrentFrame() > 32)
			{

			}

			if (e.type == Simulation::EventTypes::MouseMove && GetCursorLocked())
			{
			}


			if (e.type == Simulation::EventTypes::MouseScroll && !ImGui::GetIO().WantCaptureMouse)
			{
			}

			if (e.type == Simulation::EventTypes::WindowResize)
			{
			}

			if (e.type == Simulation::EventTypes::KeyPress && e.key == GLFW_KEY_ESCAPE) {
				exit(0);
			}

			if (e.type == Simulation::EventTypes::KeyPress && e.key == GLFW_KEY_F1)
			{
				this->SetCursorLocked(!this->GetCursorLocked());
			}
			
			if (e.type == Simulation::EventTypes::KeyPress && e.key == GLFW_KEY_D)
			{
				std::cout << "\nDRAWING MODE OVER\n";
				DrawingMode = !DrawingMode;
			}
			
			if (e.type == Simulation::EventTypes::KeyPress && e.key == GLFW_KEY_F)
			{
				if (!DrawingMode)
					ComputerDrawMode = !ComputerDrawMode;
			}

			if (e.type == Simulation::EventTypes::KeyPress && e.key == GLFW_KEY_R)
			{
				Samples.clear();
			}

			if (e.type == Simulation::EventTypes::KeyPress && e.key == GLFW_KEY_G)
			{
				ShouldGenDFT = true;
			}


			if (e.type == Simulation::EventTypes::KeyPress && e.key == GLFW_KEY_F2 && this->GetCurrentFrame() > 5)
			{
				Simulation::ShaderManager::RecompileShaders();
			}

			if (e.type == Simulation::EventTypes::KeyPress && e.key == GLFW_KEY_F3 && this->GetCurrentFrame() > 5)
			{
				Simulation::ShaderManager::ForceRecompileShaders();
			}


		}


	};

	void Pipeline::StartPipeline()
	{
		DrawingMode = true;
		Samples.reserve(MaxSamples);

		std::cout << "\nDRAWING MODE IS ACTIVE\n";

		// Application
		RayTracerApp app;
		app.Initialize();
		app.SetCursorLocked(false);
		app.vsync = true;

		// Create VBO and VAO for drawing the screen-sized quad.
		GLClasses::VertexBuffer ScreenQuadVBO;
		GLClasses::VertexArray ScreenQuadVAO;

		// Setup screensized quad for rendering
		{
			unsigned long long CurrentFrame = 0;
			float QuadVertices_NDC[] =
			{
				-1.0f,  1.0f,  0.0f, 1.0f, -1.0f, -1.0f,  0.0f, 0.0f,
				 1.0f, -1.0f,  1.0f, 0.0f, -1.0f,  1.0f,  0.0f, 1.0f,
				 1.0f, -1.0f,  1.0f, 0.0f,  1.0f,  1.0f,  1.0f, 1.0f
			};

			ScreenQuadVAO.Bind();
			ScreenQuadVBO.Bind();
			ScreenQuadVBO.BufferData(sizeof(QuadVertices_NDC), QuadVertices_NDC, GL_STATIC_DRAW);
			ScreenQuadVBO.VertexAttribPointer(0, 2, GL_FLOAT, 0, 4 * sizeof(GLfloat), 0);
			ScreenQuadVBO.VertexAttribPointer(1, 2, GL_FLOAT, 0, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));
			ScreenQuadVAO.Unbind();
		}

		// Create Shaders 
		ShaderManager::CreateShaders();

		// Shaders
		GLClasses::Shader& BlitShader = ShaderManager::GetShader("BLIT");
		GLClasses::Shader& RenderShader = ShaderManager::GetShader("RENDER");

		// Matrices
		float OrthographicRange = 400.0f;
		OrthographicCamera Orthographic(-400.0f, 400.0f, -400.0f, 400.0f);

		// Create Random Objects 
		Random RNG;
		Objects.resize(MaxSamples);

		// Clear simulation map
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		while (!glfwWindowShouldClose(app.GetWindow())) {

			glDisable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);

			app.OnUpdate();

			{
				double xpos, ypos;
				glfwGetCursorPos(app.GetWindow(), &xpos, &ypos);
				CurrentCursorPos = { xpos,ypos };
			}

			// Add sample
			if (Samples.size() < MaxSamples && DrawingMode && app.GetCurrentFrame() > 60)
			{
				if (CurrentCursorPos != PrevCursorPos)
				{
					glm::vec2 CPos = CurrentCursorPos;
					int Substeps = 32;
					for (int i = 1; i <= Substeps; i++) {
						CPos = glm::mix(PrevCursorPos, CurrentCursorPos, float(i) / float(Substeps));
						Sample s;
						s.Position = { CPos.x,app.GetHeight() - CPos.y };
						s.Position /= glm::vec2(app.GetWidth(), app.GetHeight());
						s.Position = s.Position * 2.0f - 1.0f;
						s.Active = glfwGetMouseButton(app.GetWindow(), GLFW_MOUSE_BUTTON_LEFT);
						
						Samples.push_back(s);
					}
				}

				
			}

			if (!DrawingMode && ShouldGenDFT) {
				ShouldGenDFT = false;

				float DFT_Time = glfwGetTime();

				int N = Samples.size();

				ImageDFT.CreateTransform(Samples, DFTQ);
				Samples.clear();
				ImageDFT.InverseTransform(Samples, N);

				float DFT_Time2 = glfwGetTime();

				DFTDeltaTime = DFT_Time2 - DFT_Time;

				std::cout << "\nGENERATED DFT IN " << DFTDeltaTime << " s\n";

				ComputerDrawObjects = 0;
				ComputerDrawMode = true;
			}



			///////////////
			// RENDERING //
			///////////////
			{
				int CurrentObjectCount = glm::min(Samples.size(), (size_t)MaxSamples);

				for (int i = 0; i < CurrentObjectCount; i++) {
					glm::vec4& Pos = Objects[i].Position;
					Pos.x = (Samples[i].Position.x) * (OrthographicRange - 1.0f);
					Pos.y = (Samples[i].Position.y) * (OrthographicRange - 1.0f);
					Pos.w = Samples[i].Active ? 4.0f : 0.5f;
				}

				// Object SSBO
				GLuint ObjectSSBO = 0;
				glGenBuffers(1, &ObjectSSBO);
				glBindBuffer(GL_SHADER_STORAGE_BUFFER, ObjectSSBO);
				glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Object) * CurrentObjectCount, (void*)Objects.data(), GL_DYNAMIC_DRAW);
				glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

				// Rendering ->

				// Blit Final Result 
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
				glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
				glClear(GL_COLOR_BUFFER_BIT);

				RenderShader.Use();

				RenderShader.SetMatrix4("u_Projection", Orthographic.GetProjectionMatrix());
				RenderShader.SetVector2f("u_Dimensions", glm::vec2(app.GetWidth(), app.GetHeight()));
				RenderShader.SetVector2f("u_Dims", glm::vec2(app.GetWidth(), app.GetHeight()));

				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ObjectSSBO);

				ComputerDrawObjects += (CurrentObjectCount / (60 * 6));

				ScreenQuadVAO.Bind();
				glDrawArraysInstanced(GL_TRIANGLES, 0, 6, ComputerDrawMode ? std::min(ComputerDrawObjects, (long)CurrentObjectCount) : CurrentObjectCount);
				ScreenQuadVAO.Unbind();

				glUseProgram(0);
			}
			

			glFinish();
			app.FinishFrame();

			CurrentTime = glfwGetTime();
			DeltaTime = CurrentTime - Frametime;
			Frametime = glfwGetTime();
			glfwSwapInterval(app.vsync);

			GLClasses::DisplayFrameRate(app.GetWindow(), "Simulation ");
			PrevCursorPos = CurrentCursorPos;
		}

		exit(0);
	}
}