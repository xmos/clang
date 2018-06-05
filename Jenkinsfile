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
            dir("clang") {
              checkout scm
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
          dir("bin") {
            sh 'cmake ..'
            sh 'make clang-format'
          }
        }
      }
    }
    stage('Archive') {
      steps {
        dir("llvm") {
          dir("bin") {
            archiveArtifacts artifacts: 'clang-format', fingerprint: true
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
