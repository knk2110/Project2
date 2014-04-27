JCC = javac
JFLAGS = -g

default: SplashTable.class permission

SplashTable.class: 
	$(JCC) $(JFLAGS) SplashTable.java

splash: java SplashTable

permission:
	chmod +x splash

clean: 
	$(RM) *.class
