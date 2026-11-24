## Documentation

Comprehensive HTML documentation generated with Doxygen is available through two channels:

* **Precompiled package:** Download the standalone documentation archive from the [STSELib Releases](https://github.com/STMicroelectronics/STSELib/releases) page.
* **Local build:** Generate the documentation from the library repository:

	1. Download the PlantUML `.jar` file from the [PlantUML releases](https://github.com/plantuml/plantuml/releases) page.
	2. Rename the downloaded file to `plantuml.jar` and copy it to `Middleware/STSELib/doc/resources/`.
	3. Run the following commands:

		 ```bash
		 cd Middleware/STSELib/doc/resources/
		 doxygen STSELib.doxyfile
		 ```

> [!NOTE]
> Documentation build tested with **Doxygen 1.14.0** and **plantuml-mit-1.2026.9.jar** 
> 
