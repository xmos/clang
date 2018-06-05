pipeline {
  agent {
    label "lib_builder"
  }
  options {
    skipDefaultCheckout()
  }
  stages {
    stage('Checkout') {
      steps {
        sh 'git clone https://llvm.org/git/llvm.git'
        dir("llvm") {
          dir("tools") {
            checkout scm
            dir("clang") {
              dir("tools") {
                sh 'git clone https://llvm.org/git/clang-tools-extra.git extra'
              }
            }
          }
        }
      }
    }
    stage('Build') {
      steps {
        sh 'find .'
        dir("llvm") {
          sh 'mkdir bin'
          dir("bin") {
            sh 'cmake ..'
            sh 'make clang-format clang-tidy clangd'
          }
        }
      }
    }
  }
  post {
    always {
      cleanWs()
    }
  }
}
