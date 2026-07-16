pipeline {
    agent {
        label 'labo_agent'
    }
    environment {
        GIT_REPO = 'git@github.st.com:STInternal-CS-Authentication/STSELib_Validation.git'
        DATE = java.time.LocalDate.now()
    }
    stages {
        stage('Clone repository') {
            steps {
                script {
                    dir("${BUILD_ID}")
                    {
                        bat 'git clone %GIT_REPO% .'
                        bat 'git checkout origin/dev/1.2.0'
                        bat 'git submodule update --init'  
                    }
                }
            }
        }
        stage('Checkout STSELib dev/1.2.0 HEAD') {
            steps {
                dir("${BUILD_ID}/Middleware/STSELib") {
                    script {
                        if (params.STSELIB_PR?.trim()) {
                            bat "git fetch origin +refs/pull/%STSELIB_PR%/head:pr-%STSELIB_PR%"
                            bat "git checkout pr-%STSELIB_PR%"
                        } else {
                            echo "No STSELIB_PR provided, keeping submodule revision"
                        }
                    }
                }
            }
        }
        stage('Build project') {
            steps {
                dir("${BUILD_ID}") {
                    script {
                        bat '''"C:\\ST\\STM32CubeIDE_1.16.1\\STM32CubeIDE\\stm32cubeide.exe" ^
                                --launcher.suppressErrors ^
                                -nosplash ^
                                -application org.eclipse.cdt.managedbuilder.core.headlessbuild ^
                                -data .\\temp ^
                                -importAll .\\Application\\STSE_Test_project\\STM32CubeIDE^
                                -I .\\..\\..\\..\\Middleware\\EasyTest\\src ^
                                -build STSE_Test_project/Debug'''
                     }
                }
            }
        }
        stage('Perform Functional Test') {
            steps {
                dir("${BUILD_ID}") {
                    script {
                        // Get the current date in YYYY-MM-DD format
                        bat 'c:\\ST\\STM32CubeCLT_1.18.0\\STM32CubeProgrammer\\bin\\STM32_Programmer_CLI.exe -c port=SWD freq=4000 sn=0669FF564957886687062320 -w .\\Application\\STSE_Test_project\\STM32CubeIDE\\Debug\\STSE_Test_project_Debug.elf'
                        bat 'c:\\ST\\STM32CubeCLT_1.18.0\\STM32CubeProgrammer\\bin\\STM32_Programmer_CLI.exe -c port=SWD freq=4000 sn=0669FF564957886687062320 -rst'
                        try {
                            bat 'python .\\Middleware\\STSE_CI_CD\\Tools\\com_logger\\com_logger.py .\\Middleware\\STSE_CI_CD\\Tools\\com_logger\\com_logger_config.json'
                        } catch (Exception e) {
                            bat """
                                powershell -Command "python .\\Middleware\\STSE_CI_CD\\Tools\\com_logger\\md_to_html.py test_report.md STSELib_${BUILD_ID}_${DATE}_test_report.html"
                                """
                            error("Build failed due to an error: ${e.message}")
                        }
                        bat """
                            powershell -Command "python .\\Middleware\\STSE_CI_CD\\Tools\\com_logger\\md_to_html.py test_report.md STSELib_${BUILD_ID}_${DATE}_test_report.html"
                            """
                    }
                }
            }
        }
        stage('Generate Documentation') {
            steps {
                dir("${BUILD_ID}/Middleware/STSELib/doc/resources/") {
                    script {
                        bat 'doxygen STSELib.doxyfile'
                     }
                }
            }
        }
    }
    post {
        always {
            // Get the current date in YYYY-MM-DD format
            dir("${BUILD_ID}") {
                script {
                    archiveArtifacts artifacts: '*.html', fingerprint: true
                }
            }
            dir("${BUILD_ID}/Middleware/") {
                script {
                    // Remove everything in STSELib/doc/resources except __html and Pictures
                    bat '''
                        REM Remove all folders except __html and Pictures
                        for /d %%D in ("STSELib\\doc\\resources\\*") do (
                            if /i not "%%~nxD"=="__html" if /i not "%%~nxD"=="Pictures" rmdir /s /q "%%D"
                        )
                        
                        REM Remove all files except those in __html and Pictures
                        for %%F in ("STSELib\\doc\\resources\\*") do (
                            if /i not "%%~nxF"=="__html" if /i not "%%~nxF"=="Pictures" del /q "%%F"
                        )
                    '''
                    // Remove the resources@tmp folder before zipping
                    bat '''
                        REM Remove the resources@tmp folder
                        if exist "STSELib\\doc\\resources@tmp" rmdir /s /q "STSELib\\doc\\resources@tmp"
                    '''
                    // Create the documentation release package 
                    bat """
                        powershell -Command "Compress-Archive -Path 'STSELib\\doc\\*' -DestinationPath 'STSELib_nightly_${BUILD_ID}_${DATE}_documentation.zip' -Force"
                    """
                    // Remove unwanted files and folders from STSELib
                    bat '''
                        REM Remove unwanted folders
                        if exist "STSELib\\.github" rmdir /s /q "STSELib\\.github"
                        if exist "STSELib\\.vscode" rmdir /s /q "STSELib\\.vscode"
                        if exist "STSELib\\doc" rmdir /s /q "STSELib\\doc"
                        REM Remove unwanted files
                        if exist "STSELib\\.gitignore" del /q "STSELib\\.gitignore"
                        if exist "STSELib\\.gitmodules" del /q "STSELib\\.gitmodules"
                        if exist "STSELib\\.pre-commit-config.yaml" del /q "STSELib\\.pre-commit-config.yaml"
                        if exist "STSELib\\CODE_OF_CONDUCT.md" del /q "STSELib\\CODE_OF_CONDUCT.md"
                        if exist "STSELib\\CONTRIBUTING.md" del /q "STSELib\\CONTRIBUTING.md"
                    '''
                    // Create the release file including build number and date
                    bat """
                        powershell -Command "Compress-Archive -Path 'STSELib\\*' -DestinationPath 'STSELib_nightly_${BUILD_ID}_${DATE}.zip' -Force"
                    """
                    // Archive the release in Jenkins
                    archiveArtifacts artifacts: "STSELib_nightly_${BUILD_ID}_*", fingerprint: true
                }
            }
            // Clean-up the workspace
            deleteDir()
        }
    }
}
