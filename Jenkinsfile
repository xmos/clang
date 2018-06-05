pipeline {
  agent none
  options {
    skipDefaultCheckout()
  }
  stages {
    stage('Centos 7') {
      agent {
        label "vagrant&&linux"
      }
      steps {
        vagrant(
          boxname: 'centos7-tools-next',
          boxurl: 'http://srv-bri-repo0.xmos.local/repos/boxes/builders/tools/centos/7',
          cpuCount: '1',
          memMB: '4096'
        ) {
          sh """git clone https://llvm.org/git/llvm.git"""
          dir("llvm") {
            dir("tools") {
              checkout scm
              dir("clang") {
                dir("tools") {
                  sh """git clone https://llvm.org/git/clang-tools-extra.git extra"""
                }
              }
            }
          }
          dir("llvm") {
            sh """mkdir bin"""
            dir("bin") {
              sh """cmake .."""
              sh """make clang-format clang-tidy clangd"""
            }
          }
        }
        post {
          always {
            cleanWs()
          }
        }
      }
    }
  }
  post {
    always {
      deleteDir()
    }
  }
}
