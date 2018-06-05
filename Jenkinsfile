pipeline {
  options {
    skipDefaultCheckout()
  }
  stages {
    stage('Checkout') {
      steps {
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
      }
    }
  }
}
