#include <FECS/FECS.h>
#include <atomic>
#include <iostream>
#include <string>

// --- Main ---
auto main() -> int
{
    FECS::World world;
    int registration_order_counter = 1;

    // These atomic counters will track the actual execution order of systems in each phase.
    // We use atomic integers to ensure thread-safe increments, as systems might run in parallel.
    std::atomic<int> startup_execution_counter = 1;
    std::atomic<int> runtime_execution_counter = 1;
    std::atomic<int> cleanup_execution_counter = 1;

    std::cout << "--- Registering Systems ---" << std::endl;
    std::cout << "Systems are registered in a specific order, but the scheduler will resolve the correct execution order based on dependencies." << std::endl;

    // --- Startup Phase ---
    // Registering startup systems in a non-sequential order to demonstrate the scheduler's sorting capabilities.
    std::cout << "\n-- Startup Systems --" << std::endl;
    int reg_order_1 = registration_order_counter++;
    world.Scheduler().AddSystem().Startup().Name("LoadAssets").Build([&startup_execution_counter, reg_order_1]() {
        std::cout << "[STARTUP] Executed Task #" << startup_execution_counter++ << " (Registered as #" << reg_order_1 << ", LoadAssets)" << std::endl;
    });
    std::cout << "Registered task #" << reg_order_1 << ": LoadAssets (Startup)" << std::endl;

    int reg_order_2 = registration_order_counter++;
    world.Scheduler().AddSystem().Startup().Name("ConnectToDatabase").Before("LoadAssets").Build([&startup_execution_counter, reg_order_2]() {
        std::cout << "[STARTUP] Executed Task #" << startup_execution_counter++ << " (Registered as #" << reg_order_2 << ", ConnectToDatabase)" << std::endl;
    });
    std::cout << "Registered task #" << reg_order_2 << ": ConnectToDatabase (Startup, Before: LoadAssets)" << std::endl;

    int reg_order_3 = registration_order_counter++;
    world.Scheduler().AddSystem().Startup().Name("InitializePhysics").After("LoadAssets").Build([&startup_execution_counter, reg_order_3]() {
        std::cout << "[STARTUP] Executed Task #" << startup_execution_counter++ << " (Registered as #" << reg_order_3 << ", InitializePhysics)" << std::endl;
    });
    std::cout << "Registered task #" << reg_order_3 << ": InitializePhysics (Startup, After: LoadAssets)" << std::endl;

    // --- Runtime Phase ---
    // Registering runtime systems in a non-sequential order.
    std::cout << "\n-- Runtime Systems --" << std::endl;
    int reg_order_4 = registration_order_counter++;
    world.Scheduler().AddSystem().Name("Render").After("Transform").Build([&runtime_execution_counter, reg_order_4]() {
        std::cout << "  [RUNTIME] Executed Task #" << runtime_execution_counter++ << " (Registered as #" << reg_order_4 << ", Render)" << std::endl;
    });
    std::cout << "Registered task #" << reg_order_4 << ": Render (After: Transform)" << std::endl;

    int reg_order_5 = registration_order_counter++;
    world.Scheduler().AddSystem().Name("ProcessInput").Build([&runtime_execution_counter, reg_order_5]() {
        std::cout << "  [RUNTIME] Executed Task #" << runtime_execution_counter++ << " (Registered as #" << reg_order_5 << ", ProcessInput)" << std::endl;
    });
    std::cout << "Registered task #" << reg_order_5 << ": ProcessInput" << std::endl;

    int reg_order_6 = registration_order_counter++;
    world.Scheduler().AddSystem().Name("GameLogic").After("ProcessInput").Build([&runtime_execution_counter, reg_order_6]() {
        std::cout << "  [RUNTIME] Executed Task #" << runtime_execution_counter++ << " (Registered as #" << reg_order_6 << ", GameLogic)" << std::endl;
    });
    std::cout << "Registered task #" << reg_order_6 << ": GameLogic (After: ProcessInput)" << std::endl;

    int reg_order_7 = registration_order_counter++;
    world.Scheduler().AddSystem().Name("Physics").After("GameLogic").Build([&runtime_execution_counter, reg_order_7]() {
        std::cout << "  [RUNTIME] Executed Task #" << runtime_execution_counter++ << " (Registered as #" << reg_order_7 << ", Physics)" << std::endl;
    });
    std::cout << "Registered task #" << reg_order_7 << ": Physics (After: GameLogic)" << std::endl;

    int reg_order_8 = registration_order_counter++;
    world.Scheduler().AddSystem().Name("Transform").After("Physics").Build([&runtime_execution_counter, reg_order_8]() {
        std::cout << "  [RUNTIME] Executed Task #" << runtime_execution_counter++ << " (Registered as #" << reg_order_8 << ", Transform)" << std::endl;
    });
    std::cout << "Registered task #" << reg_order_8 << ": Transform (After: Physics)" << std::endl;


    // --- Cleanup Phase ---
    // Registering cleanup systems in a non-sequential order.
    std::cout << "\n-- Cleanup Systems --" << std::endl;
    int reg_order_9 = registration_order_counter++;
    world.Scheduler().AddSystem().CleanUp().Name("ReleaseAssets").Build([&cleanup_execution_counter, reg_order_9]() {
        std::cout << "[CLEANUP] Executed Task #" << cleanup_execution_counter++ << " (Registered as #" << reg_order_9 << ", ReleaseAssets)" << std::endl;
    });
    std::cout << "Registered task #" << reg_order_9 << ": ReleaseAssets (Cleanup)" << std::endl;

    int reg_order_10 = registration_order_counter++;
    world.Scheduler().AddSystem().CleanUp().Name("DisconnectFromDatabase").After("ReleaseAssets").Build([&cleanup_execution_counter, reg_order_10]() {
        std::cout << "[CLEANUP] Executed Task #" << cleanup_execution_counter++ << " (Registered as #" << reg_order_10 << ", DisconnectFromDatabase)" << std::endl;
    });
    std::cout << "Registered task #" << reg_order_10 << ": DisconnectFromDatabase (Cleanup, After: ReleaseAssets)" << std::endl;

    int reg_order_11 = registration_order_counter++;
    world.Scheduler().AddSystem().CleanUp().Name("ShutdownPhysics").Before("ReleaseAssets").Build([&cleanup_execution_counter, reg_order_11]() {
        std::cout << "[CLEANUP] Executed Task #" << cleanup_execution_counter++ << " (Registered as #" << reg_order_11 << ", ShutdownPhysics)" << std::endl;
    });
    std::cout << "Registered task #" << reg_order_11 << ": ShutdownPhysics (Cleanup, Before: ReleaseAssets)" << std::endl;


    // --- Bake ---
    // This is where the sorting logic triggers for all phases. The scheduler resolves the execution order.
    std::cout << "\n--- Baking Schedule ---" << std::endl;
    world.Scheduler().Bake();
    std::cout << "Bake successful. System execution order is now resolved." << std::endl;

    // --- Run Startup ---
    std::cout << "\n--- Running Startup Phase ---" << std::endl;
    world.Scheduler().RunStartup();

    // --- Run Runtime ---
    // We reset the runtime execution counter before each frame to ensure the order is fresh for each tick.
    std::cout << "\n--- Running Runtime Phase (2 frames) ---" << std::endl;
    runtime_execution_counter = 1;
    world.Scheduler().Run(0.016f); // Simulate frame 1
    std::cout << "  ---" << std::endl;
    runtime_execution_counter = 1;
    world.Scheduler().Run(0.016f); // Simulate frame 2

    // --- Run Cleanup ---
    std::cout << "\n--- Running Cleanup Phase ---" << std::endl;
    world.Scheduler().RunCleanUp();

    std::cout << "\n--- Example Finished ---" << std::endl;

    return 0;
}