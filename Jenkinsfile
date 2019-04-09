pipeline {
  agent {
    label "lib_builder"
  }
  options {
    skipDefaultCheckout()
    buildDiscarder(logRotator(numToKeepStr: '10'))
  }
  triggers {
    cron('0 23 * * *')
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
        dir("llvm") {
          dir("bin") {
            sh 'cmake ..'
            sh 'make clang-tidy clang-format FormatTests -j24'
          }
        }
      }
    }
    stage('Test') {
      steps {
        sh './llvm/bin/tools/clang/unittests/Format/FormatTests'
      }
    }
    stage('Archive') {
      steps {
        dir("llvm") {
          dir("bin") {
            dir("bin") {
              archiveArtifacts artifacts: 'clang-format', fingerprint: true
              archiveArtifacts artifacts: 'clang-tidy', fingerprint: true
            }
          }
          dir("tools") {
            dir("clang") {
              dir("tools") {
                dir("clang-format") {
                  archiveArtifacts artifacts: 'clang-format-diff.py', fingerprint: true
                }
              }
            }
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
