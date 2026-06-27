# New Project Wizard

The New Project Wizard helps you create a new LOW-Engine project or open an existing project from the selected projects folder. It appears when the editor starts and can also be opened from **Project → New Project**.

## Opening the wizard

- On startup, the wizard opens automatically.
- To reopen it later, choose **Project → New Project** from the editor menu.
- The modal title is **LOWEngine Project Wizard**.

## Creating a project

1. Choose a **Project Directory**.
   - This is the parent folder where the new project folder will be created.
   - By default, the editor uses the current working directory plus `projects`.
2. Enter a **Project Name**.
   - The default name is `New Project`.
   - The name becomes both the new folder name and the `.lowproj` file name.
3. Check the availability message.
   - **Project name is available.** means the expected `.lowproj` file does not exist yet.
   - **Project with this name already exists!** means the wizard found the target `.lowproj` file and disables **Create new project**.
4. Click **Create new project**.

The wizard creates this layout:

```text
<Project Directory>/
  <Project Name>/
    <Project Name>.lowproj
```

For example, creating `Demo` in `C:/work/projects` creates:

```text
C:/work/projects/
  Demo/
    Demo.lowproj
```

After creation, the editor opens the new project immediately, saves the initial project data, pauses the current scene, changes the window title to `LOWEditor: <Project Name>`, and closes the wizard.

## Using Browse

Click **Browse** next to **Project Directory** to open the file dialog.

> **Note:** Browse can update both fields. **Project Directory** becomes the selected file's folder, and **Project Name** becomes the selected `.lowproj` filename without the extension.

Use Browse when you want to point the wizard at an existing project file or reuse a nearby project location. If you only want to type a new parent directory, edit the **Project Directory** field directly.

## Opening an existing project

The lower section, **Open existing project from selected directory:**, shows projects found under the selected **Project Directory**.

A project appears when the wizard finds a `.lowproj` file inside an immediate child folder:

```text
<Project Directory>/
  ExistingProject/
    ExistingProject.lowproj
```

Projects are displayed as icon tiles using the `.lowproj` filename without the extension. Click a tile to open that project.

After opening an existing project, the editor closes the previous project, loads the selected project, pauses the current scene, changes the window title to `LOWEditor: <project title>`, and closes the wizard.

## Closing the wizard

The **Close** button is shown only after a project is loaded. Click **Close** to dismiss the wizard without creating or opening another project.

On initial startup, before any project is loaded, the wizard may not show a **Close** button. Create or open a project to continue.

## Notes and limitations

- Project files use the `.lowproj` extension.
- Existing project discovery is one folder deep. `.lowproj` files directly inside the selected **Project Directory** are not shown, and deeper nested projects are not shown.
- The duplicate-name check only looks for `<Project Directory>/<Project Name>/<Project Name>.lowproj`.
- Empty project name or directory fields are not shown as inline errors in the wizard.
- Some creation or load failures may be reported through the editor log rather than as inline wizard messages.
- Reopening the wizard may keep values you entered previously during the same editor session.

## Troubleshooting

| Problem | What to check |
|---|---|
| **Create new project** is disabled | A project file already exists at `<Project Directory>/<Project Name>/<Project Name>.lowproj`. Change the project name or directory. |
| Existing project is not listed | Make sure the `.lowproj` file is inside an immediate child folder of the selected **Project Directory**. |
| Browse changed the project name | This is expected when selecting a `.lowproj` file; the filename becomes the project name. |
| Wizard has no Close button | Load or create a project first. The Close button only appears when a project is already loaded. |
