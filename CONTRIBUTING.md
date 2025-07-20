# Contributing

Although I do my best to achieve the goal of a nice, usable, open-source smart watch, it's not easy
to do as one person. That's why I'm open to any collobaration. There is a milestone goal that you
can find in [Milestones](https://github.com/electricalgorithm/zephyr-watch/milestones) page and
project's [Kanban board](https://github.com/users/electricalgorithm/projects/3).

Please note that I'm not a C/C++ developer with that much of experise on best practices and/or latest developments. Feel free to contact me (via e-mail or through project links) to enable better linters, force some guidelines, etc.

## Mentality

### Supported Boards First
The goal of this project is to deliver a good looking and functional open source software that can easily work on end-customer development boards. As far as I see from the market, one of the most important producer is Waveshare with its ESP32- and RP2040-based products. Let's focus on their core technology and achieve the best without adding extra hardware. I see this firmware as a easy-to-flash and easy-to-use project in future.

### Single-Responsibility Principle 
When developing a new functionality, please consider the best practices. It's suggested to discuss with a LLM about the design as well. Only thing I care is the principle of responsibility isolation between components. Let's make each subsystem "self sufficient" and testable in their own, and don't interfere with each other. In the case where we need communication between different components, you can use the `devicetwin` subsystem. It's free to add extra attributes to twin, and use them between components. Feel free to see example of `datetime` subsystem and `current_time_service` service.

## Build, Test, Push
Always make sure that you have tested your firmware in your board before creating a pull request. Since I do not have that much of time to test everything by my own, I could merge your contributions without thinking wisely. First test, and send the patch.

You can find the code workspace settings for the project in [this GitHub gist](https://gist.github.com/electricalgorithm/a154395bebc2bf2d4e7bea18f0ffdfc2).

## Squash and Merge
I don't like rebasing, sorry folks. Feel free to commit as much as you like in your branch, use any commit standard you want to use. I don't care. The most important thing when merging your changes is to use "squash" strategy and then create a commit that follows [Conventional Commit](https://www.conventionalcommits.org/en/v1.0.0/) rules.