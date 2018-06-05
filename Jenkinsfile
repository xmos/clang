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
      }
    }
    stage('Build') {
      steps {
        sh 'find .'
        dir("llvm") {
          sh 'mkdir bin'
          dir("bin") {
            sh 'cmake ..'
            sh 'make'
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
