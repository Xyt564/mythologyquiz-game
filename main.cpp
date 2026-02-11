// Modern Mythology Quiz with Dear ImGui
// Features: Smooth transitions, modern color scheme, polished UI

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <chrono>
#include <cmath>
#include <cstdlib>

// Question structure
struct Question {
    std::string question;
    std::vector<std::string> options;
    int correctAnswer;
};

// Application state
enum class AppState {
    MENU,
    QUIZ,
    RESULTS,
    DETAILS
};

// Smooth animation helper
struct SmoothValue {
    float current;
    float target;
    float speed;
    
    SmoothValue(float initial = 0.0f, float spd = 5.0f) 
        : current(initial), target(initial), speed(spd) {}
    
    void set(float value) {
        target = value;
    }
    
    void update(float deltaTime) {
        current += (target - current) * speed * deltaTime;
    }
    
    float get() const {
        return current;
    }
};

// Main Quiz Application
class MythologyQuiz {
private:
    AppState currentState = AppState::MENU;
    AppState previousState = AppState::MENU;
    
    std::vector<Question> allQuestions;
    std::vector<Question> quizQuestions;
    std::vector<int> userAnswers;
    
    int currentQuestionIndex = 0;
    int selectedAnswer = -1;
    int correctAnswers = 0;
    int totalQuestions = 0;
    float timeRemaining = 60.0f;
    float maxTime = 60.0f;
    bool timerRunning = false;
    bool showNextButton = false;
    float nextButtonDelay = 0.0f;
    
    // Animation values
    SmoothValue fadeAnimation = SmoothValue(1.0f, 3.0f);
    SmoothValue slideAnimation = SmoothValue(0.0f, 8.0f);
    SmoothValue pulseAnimation = SmoothValue(1.0f, 2.0f);
    SmoothValue progressAnimation = SmoothValue(0.0f, 3.0f);
    
    ImVec4 accentColor = ImVec4(0.24f, 0.66f, 0.99f, 1.0f);  // Modern blue
    ImVec4 successColor = ImVec4(0.29f, 0.75f, 0.55f, 1.0f); // Modern green
    ImVec4 errorColor = ImVec4(0.96f, 0.35f, 0.35f, 1.0f);   // Modern red
    ImVec4 warningColor = ImVec4(1.0f, 0.71f, 0.26f, 1.0f);  // Modern orange
    
    void initializeQuestions() {
        allQuestions = {
            {"Who is the king of the Greek gods?", {"Zeus", "Poseidon", "Hades", "Apollo"}, 0},
            {"Who is the king of the Roman gods?", {"Mars", "Jupiter", "Neptune", "Pluto"}, 1},
            {"Who is the Greek goddess of wisdom and war strategy?", {"Hera", "Artemis", "Athena", "Aphrodite"}, 2},
            {"Who is the Roman equivalent of Athena?", {"Venus", "Diana", "Minerva", "Juno"}, 2},
            {"Who is the Greek god of the sea?", {"Zeus", "Hades", "Poseidon", "Apollo"}, 2},
            {"Who is the Roman equivalent of Poseidon?", {"Jupiter", "Mars", "Mercury", "Neptune"}, 3},
            {"Who is the Greek goddess of love and beauty, born from sea foam?", {"Hera", "Athena", "Artemis", "Aphrodite"}, 3},
            {"Who is the Roman equivalent of Aphrodite?", {"Diana", "Minerva", "Juno", "Venus"}, 3},
            {"Who is the Greek god of the underworld?", {"Zeus", "Poseidon", "Hades", "Ares"}, 2},
            {"Who is the Roman equivalent of Hades?", {"Jupiter", "Neptune", "Mars", "Pluto"}, 3},
            {"Who is the Greek goddess of the harvest and agriculture?", {"Hera", "Demeter", "Artemis", "Athena"}, 1},
            {"Who is the Roman equivalent of Demeter?", {"Juno", "Venus", "Ceres", "Diana"}, 2},
            {"Who is the Greek god of war?", {"Apollo", "Hermes", "Ares", "Hephaestus"}, 2},
            {"Who is the Roman equivalent of Ares?", {"Mercury", "Mars", "Vulcan", "Apollo"}, 1},
            {"Who is the Greek messenger god, also god of travellers and thieves?", {"Apollo", "Ares", "Hermes", "Dionysus"}, 2},
            {"Who is the Roman equivalent of Hermes?", {"Mars", "Apollo", "Mercury", "Bacchus"}, 2},
            {"Who is the Greek goddess of the hunt and the moon?", {"Athena", "Hera", "Artemis", "Aphrodite"}, 2},
            {"Who is the Roman equivalent of Artemis?", {"Minerva", "Juno", "Venus", "Diana"}, 3},
            {"Who is the Greek god of fire and the forge?", {"Ares", "Apollo", "Hermes", "Hephaestus"}, 3},
            {"Who is the Roman equivalent of Hephaestus?", {"Mars", "Mercury", "Apollo", "Vulcan"}, 3},
            {"Who swallowed their children to prevent being overthrown?", {"Zeus", "Cronus/Saturn", "Poseidon", "Hades"}, 1},
            {"Who was saved by being hidden from Cronus?", {"Poseidon", "Hades", "Zeus", "Apollo"}, 2},
            {"Who gave humans fire according to Greek mythology?", {"Zeus", "Apollo", "Hermes", "Prometheus"}, 3},
            {"Who is the Greek god punished to hold up the sky forever?", {"Prometheus", "Hercules", "Atlas", "Sisyphus"}, 2},
            {"Who is the Greek goddess of marriage, often jealous of Zeus' lovers?", {"Athena", "Hera", "Artemis", "Demeter"}, 1},
            {"Who is the Roman equivalent of Hera?", {"Minerva", "Diana", "Juno", "Venus"}, 2},
            {"Who is the Greek god of wine and theatre?", {"Apollo", "Hermes", "Ares", "Dionysus"}, 3},
            {"Who is the Roman equivalent of Dionysus?", {"Mercury", "Mars", "Apollo", "Bacchus"}, 3},
            {"Who was chained to a rock with an eagle eating his liver daily?", {"Atlas", "Sisyphus", "Prometheus", "Tantalus"}, 2},
            {"Who defeated the Minotaur in Greek myth?", {"Perseus", "Hercules", "Achilles", "Theseus"}, 3},
            {"Who fought in the Trojan War and was killed by Paris' arrow?", {"Hector", "Odysseus", "Achilles", "Ajax"}, 2},
            {"Who is the Greek goddess associated with witchcraft and magic?", {"Circe", "Hecate", "Medea", "Persephone"}, 1},
            {"Who turned men into pigs in Greek mythology?", {"Medusa", "Hecate", "Circe", "Medea"}, 2},
            {"Who is the Roman goddess of the hearth, home, and family?", {"Juno", "Diana", "Minerva", "Vesta"}, 3},
            {"Who was punished to roll a boulder uphill for eternity?", {"Atlas", "Prometheus", "Tantalus", "Sisyphus"}, 3},
            {"Who is the Greek goddess of victory?", {"Athena", "Artemis", "Nike", "Hebe"}, 2},
            {"Who is the Roman equivalent of Nike?", {"Diana", "Minerva", "Juno", "Victoria"}, 3},
            {"Who is the Greek god of the sun?", {"Helios", "Apollo", "Hermes", "Ares"}, 1},
            {"Who is the Roman equivalent of Apollo?", {"Apollo", "Mars", "Mercury", "Jupiter"}, 0},
            {"Who is the Greek god of love, son of Aphrodite?", {"Cupid", "Eros", "Apollo", "Hermes"}, 1},
            {"Who is the Roman equivalent of Eros?", {"Mercury", "Cupid", "Apollo", "Mars"}, 1},
            {"Who is the Greek goddess of the rainbow and messenger of the gods?", {"Hebe", "Nike", "Iris", "Hecate"}, 2},
            {"Who is the Greek god of the forge married to Aphrodite?", {"Ares", "Apollo", "Hephaestus", "Hermes"}, 2},
            {"Who is the Roman goddess of the moon and hunting?", {"Venus", "Juno", "Minerva", "Diana"}, 3},
            {"Who is the Greek god of the sky and thunder?", {"Poseidon", "Zeus", "Hades", "Apollo"}, 1},
            {"Who is the wife of Zeus and queen of the gods?", {"Athena", "Hera", "Demeter", "Artemis"}, 1},
            {"Who is the Greek hero known for his twelve labors?", {"Perseus", "Theseus", "Achilles", "Hercules"}, 3},
            {"Who is the Greek god of dreams?", {"Hypnos", "Morpheus", "Thanatos", "Erebus"}, 1},
            {"Who is the three-headed dog guarding the underworld?", {"Chimera", "Hydra", "Cerberus", "Orthrus"}, 2},
            {"Who is the Greek goddess of youth?", {"Nike", "Iris", "Hebe", "Persephone"}, 2}
        };
    }
    
    void shuffleQuestions() {
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        std::shuffle(allQuestions.begin(), allQuestions.end(), std::default_random_engine(seed));
    }
    
    void startQuiz(int numQuestions) {
        totalQuestions = numQuestions;
        currentQuestionIndex = 0;
        correctAnswers = 0;
        userAnswers.clear();
        selectedAnswer = -1;
        
        shuffleQuestions();
        quizQuestions.clear();
        for (int i = 0; i < numQuestions && i < (int)allQuestions.size(); i++) {
            quizQuestions.push_back(allQuestions[i]);
        }
        
        changeState(AppState::QUIZ);
        loadNextQuestion();
    }
    
    void loadNextQuestion() {
        if (currentQuestionIndex >= (int)quizQuestions.size()) {
            finishQuiz();
            return;
        }
        
        selectedAnswer = -1;
        timeRemaining = maxTime;
        timerRunning = true;
        showNextButton = false;
        nextButtonDelay = 0.0f;
        slideAnimation.set(0.0f);
        progressAnimation.set((float)currentQuestionIndex / totalQuestions);
    }
    
    void submitAnswer() {
        if (selectedAnswer == -1) return;
        
        timerRunning = false;
        userAnswers.push_back(selectedAnswer);
        
        if (selectedAnswer == quizQuestions[currentQuestionIndex].correctAnswer) {
            correctAnswers++;
        }
        
        showNextButton = true;
        nextButtonDelay = 1.0f; // 1 second delay before auto-advancing
    }
    
    void nextQuestion() {
        currentQuestionIndex++;
        slideAnimation.set(1.0f);
        loadNextQuestion();
    }
    
    void finishQuiz() {
        timerRunning = false;
        changeState(AppState::RESULTS);
    }
    
    void changeState(AppState newState) {
        previousState = currentState;
        currentState = newState;
        fadeAnimation.current = 1.0f;
        fadeAnimation.target = 1.0f;
        slideAnimation.set(0.0f);
    }

public:
    MythologyQuiz() {
        initializeQuestions();
    }
    
    void update(float deltaTime) {
        fadeAnimation.update(deltaTime);
        slideAnimation.update(deltaTime);
        pulseAnimation.update(deltaTime);
        progressAnimation.update(deltaTime);
        
        // Pulse animation for emphasis
        static float pulseTime = 0.0f;
        pulseTime += deltaTime;
        pulseAnimation.set(1.0f + 0.05f * sin(pulseTime * 3.0f));
        
        // Timer countdown
        if (timerRunning && currentState == AppState::QUIZ) {
            timeRemaining -= deltaTime;
            if (timeRemaining <= 0.0f) {
                timeRemaining = 0.0f;
                timerRunning = false;
                selectedAnswer = -1;
                userAnswers.push_back(-1);
                showNextButton = true;
                nextButtonDelay = 1.0f;
            }
        }
        
        // Auto-advance to next question
        if (nextButtonDelay > 0.0f) {
            nextButtonDelay -= deltaTime;
            if (nextButtonDelay <= 0.0f && showNextButton) {
                if (currentQuestionIndex + 1 < totalQuestions) {
                    nextQuestion();
                } else {
                    finishQuiz();
                }
            }
        }
    }
    
    void render() {
        ImGuiIO& io = ImGui::GetIO();
        ImVec2 windowSize = io.DisplaySize;
        
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(windowSize);
        
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        
        ImGui::Begin("Mythology Quiz", nullptr, 
            ImGuiWindowFlags_NoTitleBar | 
            ImGuiWindowFlags_NoResize | 
            ImGuiWindowFlags_NoMove | 
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoScrollbar);
        
        ImGui::PopStyleVar(3);
        
        float alpha = fadeAnimation.get();
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
        
        switch (currentState) {
            case AppState::MENU:
                renderMenuScreen();
                break;
            case AppState::QUIZ:
                renderQuizScreen();
                break;
            case AppState::RESULTS:
                renderResultsScreen();
                break;
            case AppState::DETAILS:
                renderDetailsScreen();
                break;
        }
        
        ImGui::PopStyleVar();
        ImGui::End();
    }
    
    void renderMenuScreen() {
        ImGuiIO& io = ImGui::GetIO();
        ImVec2 windowSize = io.DisplaySize;
        
        // Calculate centered content area
        float contentWidth = fmin(800.0f, windowSize.x * 0.6f);
        float startX = (windowSize.x - contentWidth) * 0.5f;
        float startY = windowSize.y * 0.2f;
        
        ImGui::SetCursorPos(ImVec2(startX, startY));
        ImGui::BeginGroup();
        
        // Title - use huge font if available
        if (io.Fonts->Fonts.Size > 2) {
            ImGui::PushFont(io.Fonts->Fonts[2]);
        }
        ImGui::PushStyleColor(ImGuiCol_Text, accentColor);
        const char* title = "MYTHOLOGY QUIZ";
        ImVec2 titleSize = ImGui::CalcTextSize(title);
        ImGui::SetCursorPosX(startX + (contentWidth - titleSize.x) * 0.5f);
        ImGui::Text("%s", title);
        ImGui::PopStyleColor();
        if (io.Fonts->Fonts.Size > 2) {
            ImGui::PopFont();
        }
        
        ImGui::Dummy(ImVec2(0, 30));
        
        // Use larger font for subtitle
        if (io.Fonts->Fonts.Size > 1) {
            ImGui::PushFont(io.Fonts->Fonts[1]);
        }
        const char* subtitle = "Test your knowledge of Greek and Roman mythology!";
        ImVec2 subtitleSize = ImGui::CalcTextSize(subtitle);
        ImGui::SetCursorPosX(startX + (contentWidth - subtitleSize.x) * 0.5f);
        ImGui::Text("%s", subtitle);
        if (io.Fonts->Fonts.Size > 1) {
            ImGui::PopFont();
        }
        
        ImGui::Dummy(ImVec2(0, 50));
        ImGui::SetCursorPosX(startX);
        ImGui::Separator();
        ImGui::Dummy(ImVec2(0, 40));
        
        const char* instruction = "Select number of questions:";
        ImVec2 instructionSize = ImGui::CalcTextSize(instruction);
        ImGui::SetCursorPosX(startX + (contentWidth - instructionSize.x) * 0.5f);
        ImGui::Text("%s", instruction);
        
        ImGui::Dummy(ImVec2(0, 30));
        
        ImVec2 buttonSize(contentWidth, 70.0f);
        
        ImGui::SetCursorPosX(startX);
        if (ImGui::Button("5 Questions", buttonSize)) {
            startQuiz(5);
        }
        ImGui::Dummy(ImVec2(0, 15));
        
        ImGui::SetCursorPosX(startX);
        if (ImGui::Button("10 Questions", buttonSize)) {
            startQuiz(10);
        }
        ImGui::Dummy(ImVec2(0, 15));
        
        ImGui::SetCursorPosX(startX);
        if (ImGui::Button("15 Questions", buttonSize)) {
            startQuiz(15);
        }
        ImGui::Dummy(ImVec2(0, 15));
        
        ImGui::SetCursorPosX(startX);
        if (ImGui::Button("25 Questions", buttonSize)) {
            startQuiz(25);
        }
        ImGui::Dummy(ImVec2(0, 15));
        
        ImGui::SetCursorPosX(startX);
        if (ImGui::Button("50 Questions (Full Quiz)", buttonSize)) {
            startQuiz(50);
        }
        
        ImGui::EndGroup();
    }
    
    void renderQuizScreen() {
        if (currentQuestionIndex >= (int)quizQuestions.size()) return;
        
        ImGuiIO& io = ImGui::GetIO();
        ImVec2 windowSize = io.DisplaySize;
        
        Question& q = quizQuestions[currentQuestionIndex];
        
        // Calculate centered content area
        float contentWidth = fmin(1000.0f, windowSize.x * 0.65f);
        float startX = (windowSize.x - contentWidth) * 0.5f;
        float startY = windowSize.y * 0.15f;
        
        ImGui::SetCursorPos(ImVec2(startX, startY));
        ImGui::BeginGroup();
        
        // Progress bar
        float progress = progressAnimation.get();
        ImGui::Text("Question %d / %d", currentQuestionIndex + 1, totalQuestions);
        ImGui::SetCursorPosX(startX);
        ImGui::ProgressBar(progress, ImVec2(contentWidth, 8.0f));
        ImGui::Dummy(ImVec2(0, 20));
        
        // Timer with color coding
        float timerRatio = timeRemaining / maxTime;
        ImVec4 timerColor;
        if (timerRatio > 0.5f) {
            timerColor = successColor;
        } else if (timerRatio > 0.25f) {
            timerColor = warningColor;
        } else {
            timerColor = errorColor;
        }
        
        ImGui::PushStyleColor(ImGuiCol_Text, timerColor);
        ImGui::Text("Time: %.0f seconds", timeRemaining);
        ImGui::PopStyleColor();
        
        ImGui::Dummy(ImVec2(0, 15));
        ImGui::SetCursorPosX(startX);
        ImGui::Separator();
        ImGui::Dummy(ImVec2(0, 35));
        
        // Question - use large font
        if (io.Fonts->Fonts.Size > 1) {
            ImGui::PushFont(io.Fonts->Fonts[1]);
        }
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
        ImGui::PushTextWrapPos(startX + contentWidth);
        ImGui::SetCursorPosX(startX);
        ImGui::TextWrapped("%s", q.question.c_str());
        ImGui::PopTextWrapPos();
        ImGui::PopStyleColor();
        if (io.Fonts->Fonts.Size > 1) {
            ImGui::PopFont();
        }
        
        ImGui::Dummy(ImVec2(0, 40));
        
        // Answer options
        for (int i = 0; i < (int)q.options.size(); i++) {
            ImGui::PushID(i);
            
            bool isSelected = (selectedAnswer == i);
            ImVec4 buttonColor = isSelected ? accentColor : ImVec4(0.2f, 0.25f, 0.3f, 1.0f);
            
            ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
            ImVec4 hoverColor = ImVec4(buttonColor.x * 1.2f, buttonColor.y * 1.2f, buttonColor.z * 1.2f, buttonColor.w);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hoverColor);
            ImVec4 activeColor = ImVec4(buttonColor.x * 0.8f, buttonColor.y * 0.8f, buttonColor.z * 0.8f, buttonColor.w);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, activeColor);
            
            ImGui::SetCursorPosX(startX);
            bool clicked = ImGui::Button(q.options[i].c_str(), ImVec2(contentWidth, 65.0f));
            
            ImGui::PopStyleColor(3);
            
            if (clicked && !showNextButton) {
                selectedAnswer = i;
                submitAnswer();
            }
            
            ImGui::Dummy(ImVec2(0, 12));
            ImGui::PopID();
        }
        
        ImGui::EndGroup();
    }
    
    void renderResultsScreen() {
        ImGuiIO& io = ImGui::GetIO();
        ImVec2 windowSize = io.DisplaySize;
        
        float contentWidth = fmin(800.0f, windowSize.x * 0.6f);
        float startX = (windowSize.x - contentWidth) * 0.5f;
        float startY = windowSize.y * 0.2f;
        
        ImGui::SetCursorPos(ImVec2(startX, startY));
        ImGui::BeginGroup();
        
        // Title - use huge font if available
        if (io.Fonts->Fonts.Size > 2) {
            ImGui::PushFont(io.Fonts->Fonts[2]);
        }
        ImGui::PushStyleColor(ImGuiCol_Text, accentColor);
        const char* title = "QUIZ RESULTS";
        ImVec2 titleSize = ImGui::CalcTextSize(title);
        ImGui::SetCursorPosX(startX + (contentWidth - titleSize.x) * 0.5f);
        ImGui::Text("%s", title);
        ImGui::PopStyleColor();
        if (io.Fonts->Fonts.Size > 2) {
            ImGui::PopFont();
        }
        
        ImGui::Dummy(ImVec2(0, 40));
        ImGui::SetCursorPosX(startX);
        ImGui::Separator();
        ImGui::Dummy(ImVec2(0, 40));
        
        // Score
        float scorePercent = (float)correctAnswers / totalQuestions * 100.0f;
        
        const char* performance;
        ImVec4 performanceColor;
        
        if (scorePercent >= 90.0f) {
            performance = "EXCELLENT!";
            performanceColor = successColor;
        } else if (scorePercent >= 75.0f) {
            performance = "GREAT!";
            performanceColor = successColor;
        } else if (scorePercent >= 60.0f) {
            performance = "GOOD!";
            performanceColor = warningColor;
        } else {
            performance = "KEEP PRACTICING!";
            performanceColor = errorColor;
        }
        
        if (io.Fonts->Fonts.Size > 1) {
            ImGui::PushFont(io.Fonts->Fonts[1]);
        }
        ImGui::PushStyleColor(ImGuiCol_Text, performanceColor);
        ImVec2 perfSize = ImGui::CalcTextSize(performance);
        ImGui::SetCursorPosX(startX + (contentWidth - perfSize.x) * 0.5f);
        ImGui::Text("%s", performance);
        ImGui::PopStyleColor();
        if (io.Fonts->Fonts.Size > 1) {
            ImGui::PopFont();
        }
        
        ImGui::Dummy(ImVec2(0, 30));
        
        char scoreText[64];
        snprintf(scoreText, sizeof(scoreText), "You got %d out of %d correct", correctAnswers, totalQuestions);
        ImVec2 scoreSize = ImGui::CalcTextSize(scoreText);
        ImGui::SetCursorPosX(startX + (contentWidth - scoreSize.x) * 0.5f);
        ImGui::Text("%s", scoreText);
        
        ImGui::Dummy(ImVec2(0, 15));
        
        char percentText[64];
        snprintf(percentText, sizeof(percentText), "Score: %.1f%%", scorePercent);
        ImVec2 percentSize = ImGui::CalcTextSize(percentText);
        ImGui::SetCursorPosX(startX + (contentWidth - percentSize.x) * 0.5f);
        ImGui::Text("%s", percentText);
        
        ImGui::Dummy(ImVec2(0, 50));
        ImGui::SetCursorPosX(startX);
        ImGui::Separator();
        ImGui::Dummy(ImVec2(0, 40));
        
        ImVec2 buttonSize(contentWidth, 70.0f);
        
        ImGui::SetCursorPosX(startX);
        if (ImGui::Button("View Detailed Results", buttonSize)) {
            changeState(AppState::DETAILS);
        }
        
        ImGui::Dummy(ImVec2(0, 15));
        
        ImGui::SetCursorPosX(startX);
        if (ImGui::Button("Back to Menu", buttonSize)) {
            changeState(AppState::MENU);
        }
        
        ImGui::EndGroup();
    }
    
    void renderDetailsScreen() {
        ImGuiIO& io = ImGui::GetIO();
        ImVec2 windowSize = io.DisplaySize;
        
        float contentWidth = fmin(1000.0f, windowSize.x * 0.7f);
        float startX = (windowSize.x - contentWidth) * 0.5f;
        float startY = windowSize.y * 0.08f;
        
        ImGui::SetCursorPos(ImVec2(startX, startY));
        ImGui::BeginGroup();
        
        // Title - use huge font if available
        if (io.Fonts->Fonts.Size > 2) {
            ImGui::PushFont(io.Fonts->Fonts[2]);
        }
        ImGui::PushStyleColor(ImGuiCol_Text, accentColor);
        const char* title = "DETAILED RESULTS";
        ImVec2 titleSize = ImGui::CalcTextSize(title);
        ImGui::SetCursorPosX(startX + (contentWidth - titleSize.x) * 0.5f);
        ImGui::Text("%s", title);
        ImGui::PopStyleColor();
        if (io.Fonts->Fonts.Size > 2) {
            ImGui::PopFont();
        }
        
        ImGui::Dummy(ImVec2(0, 25));
        ImGui::SetCursorPosX(startX);
        ImGui::Separator();
        ImGui::Dummy(ImVec2(0, 20));
        
        // Scrollable content
        ImGui::SetCursorPosX(startX);
        float scrollHeight = windowSize.y - startY - 150;
        ImGui::BeginChild("DetailedResults", ImVec2(contentWidth, scrollHeight), true);
        
        for (int i = 0; i < (int)quizQuestions.size(); i++) {
            Question& q = quizQuestions[i];
            int userAnswer = (i < (int)userAnswers.size()) ? userAnswers[i] : -1;
            bool correct = (userAnswer == q.correctAnswer);
            
            const char* statusText = correct ? "✓ CORRECT" : "✗ INCORRECT";
            ImVec4 statusColor = correct ? successColor : errorColor;
            
            ImGui::PushStyleColor(ImGuiCol_Text, statusColor);
            ImGui::Text("%d. %s", i + 1, statusText);
            ImGui::PopStyleColor();
            
            ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + contentWidth - 60.0f);
            ImGui::Text("Q: %s", q.question.c_str());
            ImGui::PopTextWrapPos();
            
            std::string yourAnswer = (userAnswer >= 0 && userAnswer < (int)q.options.size()) 
                ? q.options[userAnswer] 
                : "No answer (time expired)";
            std::string correctAnswer = q.options[q.correctAnswer];
            
            if (!correct) {
                ImGui::PushStyleColor(ImGuiCol_Text, errorColor);
                ImGui::Text("Your answer: %s", yourAnswer.c_str());
                ImGui::PopStyleColor();
                
                ImGui::PushStyleColor(ImGuiCol_Text, successColor);
                ImGui::Text("Correct answer: %s", correctAnswer.c_str());
                ImGui::PopStyleColor();
            } else {
                ImGui::PushStyleColor(ImGuiCol_Text, successColor);
                ImGui::Text("Your answer: %s", yourAnswer.c_str());
                ImGui::PopStyleColor();
            }
            
            ImGui::Dummy(ImVec2(0, 15));
            ImGui::Separator();
            ImGui::Dummy(ImVec2(0, 15));
        }
        
        ImGui::EndChild();
        
        ImGui::Dummy(ImVec2(0, 20));
        
        ImGui::SetCursorPosX(startX);
        if (ImGui::Button("Back to Results", ImVec2(contentWidth, 70.0f))) {
            changeState(AppState::RESULTS);
        }
        
        ImGui::EndGroup();
    }
};

// Main application
int main(int, char**) {
    // Setup window
    if (!glfwInit())
        return 1;

    // GL 3.3 + GLSL 330
    const char* glsl_version = "#version 330";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Get window size from environment variables or use defaults
    int windowWidth = 1400;
    int windowHeight = 900;
    
    const char* envWidth = getenv("MYTHOLOGY_QUIZ_WIDTH");
    const char* envHeight = getenv("MYTHOLOGY_QUIZ_HEIGHT");
    
    if (envWidth != nullptr) {
        int w = atoi(envWidth);
        if (w >= 1200 && w <= 7680) { // Sanity check
            windowWidth = w;
        }
    }
    
    if (envHeight != nullptr) {
        int h = atoi(envHeight);
        if (h >= 800 && h <= 4320) { // Sanity check
            windowHeight = h;
        }
    }

    // Create window
    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Mythology Quiz - Modern Edition", nullptr, nullptr);
    if (window == nullptr)
        return 1;
    
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    
    // Load fonts with high resolution
    ImFontConfig font_config;
    font_config.OversampleH = 3;
    font_config.OversampleV = 3;
    font_config.PixelSnapH = false;
    
    // Try to load system fonts, fallback to default if not available
    ImFont* defaultFont = nullptr;
    ImFont* largeFont = nullptr;
    ImFont* hugeFont = nullptr;
    
    // List of fonts to try (in order of preference)
    const char* fontPaths[] = {
        // Linux paths
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/TTF/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
        "/usr/share/fonts/liberation/LiberationSans-Regular.ttf",
        // macOS paths
        "/System/Library/Fonts/Helvetica.ttc",
        "/System/Library/Fonts/SFNS.ttf",
        "/Library/Fonts/Arial.ttf",
        nullptr
    };
    
    bool fontLoaded = false;
    for (int i = 0; fontPaths[i] != nullptr && !fontLoaded; i++) {
        FILE* fontFile = fopen(fontPaths[i], "rb");
        if (fontFile) {
            fclose(fontFile);
            defaultFont = io.Fonts->AddFontFromFileTTF(fontPaths[i], 24.0f, &font_config);
            largeFont = io.Fonts->AddFontFromFileTTF(fontPaths[i], 36.0f, &font_config);
            hugeFont = io.Fonts->AddFontFromFileTTF(fontPaths[i], 48.0f, &font_config);
            if (defaultFont && largeFont && hugeFont) {
                fontLoaded = true;
            }
        }
    }
    
    // Fallback to default font if no system font found
    if (!fontLoaded) {
        font_config.SizePixels = 24.0f;
        defaultFont = io.Fonts->AddFontDefault(&font_config);
        font_config.SizePixels = 36.0f;
        largeFont = io.Fonts->AddFontDefault(&font_config);
        font_config.SizePixels = 48.0f;
        hugeFont = io.Fonts->AddFontDefault(&font_config);
    }
    
    // Build font atlas
    io.Fonts->Build();

    // Setup Dear ImGui style - Modern dark theme
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    
    // Modern styling
    style.WindowRounding = 12.0f;
    style.FrameRounding = 8.0f;
    style.ScrollbarRounding = 12.0f;
    style.GrabRounding = 8.0f;
    style.PopupRounding = 8.0f;
    style.WindowPadding = ImVec2(20, 20);
    style.FramePadding = ImVec2(10, 6);
    style.ItemSpacing = ImVec2(10, 8);
    
    // Modern color scheme
    ImVec4* colors = style.Colors;
    colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.11f, 0.13f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.12f, 0.13f, 0.15f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.10f, 0.11f, 0.13f, 0.95f);
    colors[ImGuiCol_Border] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.18f, 0.20f, 0.25f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.26f, 0.32f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.28f, 0.30f, 0.37f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.11f, 0.13f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.15f, 0.16f, 0.19f, 1.00f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.15f, 0.16f, 0.19f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.11f, 0.13f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.28f, 0.30f, 0.37f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.35f, 0.38f, 0.46f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.42f, 0.45f, 0.55f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.24f, 0.66f, 0.99f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.66f, 0.99f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.20f, 0.56f, 0.89f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.24f, 0.66f, 0.99f, 0.80f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.66f, 0.99f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.56f, 0.89f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.24f, 0.66f, 0.99f, 0.55f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.24f, 0.66f, 0.99f, 0.80f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.24f, 0.66f, 0.99f, 1.00f);
    colors[ImGuiCol_Separator] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.24f, 0.66f, 0.99f, 0.78f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.24f, 0.66f, 0.99f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.24f, 0.66f, 0.99f, 0.25f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.24f, 0.66f, 0.99f, 0.67f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.24f, 0.66f, 0.99f, 0.95f);
    colors[ImGuiCol_Tab] = ImVec4(0.18f, 0.20f, 0.25f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.24f, 0.66f, 0.99f, 0.80f);
    colors[ImGuiCol_TabActive] = ImVec4(0.24f, 0.66f, 0.99f, 1.00f);
    colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.24f, 0.66f, 0.99f, 0.35f);

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Our application
    MythologyQuiz quiz;

    // Main loop
    float lastTime = (float)glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Calculate delta time
        float currentTime = (float)glfwGetTime();
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Update and render quiz
        quiz.update(deltaTime);
        quiz.render();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.10f, 0.11f, 0.13f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
