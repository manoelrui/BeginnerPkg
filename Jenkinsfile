#!/usr/bin/env groovy

node {

    checkout scm    
}

pipeline {
    agent any

    stages {
        stage('Build') {
            steps {
                echo 'Building..'    
            }
        }    
        stage('Test') {
            steps {
                echo 'Testing..'    
            }    
        }
        stage('Deploy') {
            steps {
                echo 'Deploying....'    
            }
        }
    }
}
