class Project{
public:
    std::string open_file_dialog_path = "";
    bool is_open_file_dialog_open = false;

    bool view_calculator = false;
    std::string calculator_expression;

    ProjectSettings project_settings;

    //The current working directory for the projekt. Not for this program.
    std::string opened_directory_path = "";

    //Things that could be included in the new Project class, or at least should disappear when a new project is opened

    std::map<std::string, std::unique_ptr<AdvancedActorSpawner>> spawnable_actor_map;

    std::unique_ptr<FileNode> opened_directory = nullptr;
    std::vector<std::unique_ptr<Tab>> tabs;
    Tab* active_tab = nullptr;

    std::string currently_selected_actor_type = "";

    int actor_count_for_naming_purposes = 0;

};
