/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Post.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ilbendib <ilbendib@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/07 15:56:48 by ilbendib          #+#    #+#             */
/*   Updated: 2024/11/07 15:57:48 by ilbendib         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef POST_HPP
# define POST_HPP


#include "include.hpp"

class Post
{
	public:
		Post();
		~Post();
		Post(const Post &copy);
		Post &operator=(const Post &copy);
};

#endif